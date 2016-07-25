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
#include "macros.h"
#include "scoped_fd.h"
#include "term.h"

namespace zi {

class File {
 public:
  File();
  ~File();

  void Clear();
  void Read(const std::string& path);

  void Display(term::CommandBuffer* commands, int line_index);

  size_t line_count() const { return lines_.size(); }

 private:
  std::vector<std::string> lines_;
  bool has_terminating_newline_ = true;

  DISALLOW_COPY_AND_ASSIGN(File);
};

File::File() {}

File::~File() {}

void File::Clear() {
  lines_.clear();
  has_terminating_newline_ = true;
}

void File::Read(const std::string& path) {
  Clear();
  ScopedFD fd(HANDLE_EINTR(open(path.c_str(), O_RDONLY)));
  // TODO(abarth): Add an error reporting mechanism.
  if (!fd.is_valid())
    return;
  std::string current_line;
  constexpr size_t kBufferSize = 1 << 16;
  char buffer[kBufferSize];
  for (;;) {
    int count = HANDLE_EINTR(read(fd.get(), buffer, kBufferSize));
    if (count == -1)
      return;
    if (count == 0)
      break;
    const char* pos = buffer;
    const char* end = pos + count;
    while (pos != end) {
      const char* newline = std::find(pos, end, '\n');
      if (!newline)
        break;
      current_line.append(pos, newline);
      lines_.push_back(std::move(current_line));
      current_line.clear();
      pos = newline + 1;
    }
    current_line.append(pos, end);
  }
  if (!current_line.empty()) {
    has_terminating_newline_ = false;
    lines_.push_back(std::move(current_line));
  }
}

void File::Display(term::CommandBuffer* commands, int line_index) {
  const std::string& line = lines_[line_index];
  // TODO(abarth): What if we don't want to fill the entire terminal?
  size_t count = std::min(line.size(), term::cols);
  commands->Write(line.data(), count);
  *commands << term::kEraseToEndOfLine;
}

class Shell {
 public:
  Shell();
  ~Shell();

  void OpenFile(const std::string& path);

  int Run();

  void set_status(std::string status) { status_ = std::move(status); }

 private:
  void Display();

  std::string status_;
  std::vector<std::unique_ptr<File>> files_;
  size_t current_file_ = 0;

  DISALLOW_COPY_AND_ASSIGN(Shell);
};

Shell::Shell() {
  term::Put(term::kSaveScreen);
  term::Put(term::kMoveCursorHome);
}

Shell::~Shell() {
  term::Put(term::kRestoreScreen);
}

void Shell::OpenFile(const std::string& path) {
  std::unique_ptr<File> file(new File());
  file->Read(path);
  current_file_ = files_.size();
  files_.push_back(std::move(file));
}

int Shell::Run() {
  Display();
  for (;;) {
    char c = '\0';
    int count = read(STDIN_FILENO, &c, 1);
    if (count == -1)
      return 1;
    if (count == 0)
      continue;
    switch (c) {
      case 'q':
        return 0;
      case 'h':
        term::Put(term::kMoveCursorLeft);
        continue;
      case 'j':
        term::Put(term::kMoveCursorDown);
        continue;
      case 'k':
        term::Put(term::kMoveCursorUp);
        continue;
      case 'l':
        term::Put(term::kMoveCursorRight);
        continue;
      default:
        status_ = c;
    }
    Display();
  }
  return 0;
}

void Shell::Display() {
  term::CommandBuffer commands;
  commands << term::kSaveCursorPosition << term::kEraseScreen
           << term::kMoveCursorHome;
  if (!files_.empty()) {
    auto& file = files_[current_file_];
    for (size_t i = 0; i < term::rows - 1; ++i) {
      commands.MoveCursorTo(0, i);
      if (i < file->line_count())
        file->Display(&commands, i);
      else
        commands << term::kSetLowIntensity << "~"
                 << term::kClearCharacterAttributes << term::kEraseToEndOfLine;
    }
  }
  commands.MoveCursorTo(0, term::rows - 1);
  commands << status_ << term::kEraseToEndOfLine
           << term::kRestoreCursorPosition;
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
    shell.set_status(std::move(file_name));
  } else {
    shell.set_status("No file.");
  }
  return shell.Run();
}
