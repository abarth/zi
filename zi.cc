// Copyright (c) 2016, Google Inc.
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "command_buffer.h"
#include "editor.h"
#include "scoped_fd.h"
#include "term.h"
#include "text/text_buffer.h"
#include "zen/macros.h"

namespace zi {

std::vector<char> ReadFile(const std::string& path) {
  std::vector<char> result;
  ScopedFD fd(HANDLE_EINTR(open(path.c_str(), O_RDONLY)));
  // TODO(abarth): Add an error reporting mechanism.
  if (!fd.is_valid())
    return result;
  constexpr size_t kBufferSize = 1 << 16;
  char buffer[kBufferSize];
  for (;;) {
    int count = HANDLE_EINTR(read(fd.get(), buffer, kBufferSize));
    if (count == -1)
      return result;
    if (count == 0)
      break;
    result.insert(result.end(), buffer, buffer + count);
  }
  return result;
}

bool WriteFileDescriptor(int fd, const char* data, ssize_t size) {
  ssize_t total = 0;
  for (ssize_t partial = 0; total < size; total += partial) {
    partial = HANDLE_EINTR(write(fd, data + total, size - total));
    if (partial < 0)
      return false;
  }
  return true;
}

bool WriteStringViewToFileDescriptor(int fd, StringView string_view) {
  if (string_view.is_empty())
    return true;
  return WriteFileDescriptor(fd, string_view.begin(), string_view.length());
}

bool WriteAtomically(const std::string& path, TextView text) {
  // TODO(abarth): We should open this file at the start and hold onto it.
  std::string temp_path = path + ".swp";
  ScopedFD fd(HANDLE_EINTR(creat(temp_path.c_str(), 0666)));
  // TODO(abarth): Add an error reporting mechanism.
  if (!fd.is_valid())
    return false;
  if (!WriteStringViewToFileDescriptor(fd.get(), text.left()) ||
      !WriteStringViewToFileDescriptor(fd.get(), text.right())) {
    unlink(temp_path.c_str());
    return false;
  }
  return rename(temp_path.c_str(), path.c_str()) != -1;
}

enum class Mode {
  Vi,
  Command,
  Input,
};

class Shell {
 public:
  Shell();
  ~Shell();

  void OpenFile(const std::string& path);
  void Save();

  int Run();

  void mark_needs_display() { needs_display_ = true; }

 private:
  void Display();

  void HandleCharacterInViMode(char c);
  void HandleCharacterInCommandMode(char c);
  void HandleCharacterInInputMode(char c);

  void ExecuteCommand(const std::string& command);

  std::string path_;
  Mode mode_ = Mode::Vi;
  std::string status_;
  Editor editor_;

  bool should_quit_ = false;
  bool needs_display_ = false;

  DISALLOW_COPY_AND_ASSIGN(Shell);
};

Shell::Shell() {
  term::Put(term::kSaveScreen);
  term::Put(term::kMoveCursorHome);
  editor_.Resize(term::cols, term::rows);
}

Shell::~Shell() {
  term::Put(term::kRestoreScreen);
}

void Shell::OpenFile(const std::string& path) {
  std::unique_ptr<TextBuffer> text(new TextBuffer(ReadFile(path)));
  editor_.SetText(std::move(text));
  path_ = std::move(path);
}

void Shell::Save() {
  WriteAtomically(path_, editor_.text()->GetText());
}

int Shell::Run() {
  Display();
  while (!should_quit_) {
    char c = '\0';
    int count = read(STDIN_FILENO, &c, 1);
    if (count == -1)
      return 1;
    if (count == 0)
      continue;
    switch (mode_) {
      case Mode::Vi:
        HandleCharacterInViMode(c);
        break;
      case Mode::Command:
        HandleCharacterInCommandMode(c);
        break;
      case Mode::Input:
        HandleCharacterInInputMode(c);
        break;
    }
    if (needs_display_)
      Display();
  }
  return 0;
}

void Shell::HandleCharacterInViMode(char c) {
  switch (c) {
    case 'h':
      if (!editor_.MoveCursorLeft())
        term::Put(term::kBell);
      mark_needs_display();
      break;
    case 'j':
      if (!editor_.MoveCursorDown())
        term::Put(term::kBell);
      mark_needs_display();
      break;
    case 'k':
      if (!editor_.MoveCursorUp())
        term::Put(term::kBell);
      mark_needs_display();
      break;
    case 'l':
      if (!editor_.MoveCursorRight())
        term::Put(term::kBell);
      mark_needs_display();
      break;
    case 'i':
      mode_ = Mode::Input;
      break;
    case 'Z':
      should_quit_ = true;
      break;
    case ':':
      mode_ = Mode::Command;
      status_ = ":";
      mark_needs_display();
      break;
    default:
      break;
  }
}

void Shell::HandleCharacterInCommandMode(char c) {
  if (c == '\n' || c == '\r') {
    ExecuteCommand(status_);
    mark_needs_display();
  } else if (c == '\x8') {
    status_.pop_back();
    mark_needs_display();
  } else if (c == '\x1b') {
    status_.clear();
    mode_ = Mode::Vi;
    mark_needs_display();
  } else {
    status_.push_back(c);
    mark_needs_display();
  }
}

void Shell::HandleCharacterInInputMode(char c) {
  if (c == '\x09') {
    // TODO(abarth): Tab handling.
    editor_.InsertCharacter(c);
  } else if (c == '\x0A' || c == '\x0D') {
    editor_.InsertLineBreak();
  } else if (c == '\x1b') {
    mode_ = Mode::Vi;
  } else if (c >= ' ' && c < '\x7F') {
    editor_.InsertCharacter(c);
  } else if (c == '\x7F') {
    if (!editor_.Backspace())
      term::Put(term::kBell);
  } else {
    status_ = "Unknown character: " + std::to_string(c);
  }
  mark_needs_display();
}

void Shell::ExecuteCommand(const std::string& command) {
  if (command == ":q")
    should_quit_ = true;
  else if (command == ":w") {
    status_ = "Saving file to " + path_;
    Save();
  }
  mode_ = Mode::Vi;
}

void Shell::Display() {
  CommandBuffer commands;
  commands << term::kEraseScreen;
  editor_.Display(&commands);
  commands.MoveCursorTo(0, term::rows - 1);
  commands << status_ << term::kEraseToEndOfLine;
  editor_.UpdateCursor(&commands);
  commands.Execute();
}

}  // namespace zi

int main(int argc, char** argv) {
  if (!term::Init())
    return 1;
  zi::Shell shell;
  if (argc > 1) {
    std::string file_name = argv[1];
    shell.OpenFile(file_name);
  }
  return shell.Run();
}
