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
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete

#define HANDLE_EINTR(x)                                     \
  ({                                                        \
    decltype(x) eintr_wrapper_result;                       \
    do {                                                    \
      eintr_wrapper_result = (x);                           \
    } while (eintr_wrapper_result == -1 && errno == EINTR); \
    eintr_wrapper_result;                                   \
  })

#define ESC "\x1B"

namespace term {
namespace {

struct termios g_original_termios;

size_t cols = 0;
size_t rows = 0;

}  // namespace

constexpr char kBell[] = "\x07";
constexpr char kBackspace[] = "\x08";

constexpr char kReset[] = ESC "c";
constexpr char kEnableLineWrap[] = ESC "[7h";
constexpr char kDisableLineWrap[] = ESC "[7l";

constexpr char kSaveScreen[] = ESC "[?47h";
constexpr char kRestoreScreen[] = ESC "[?47l";

constexpr char kEraseToEndOfLine[] = ESC "[K";
constexpr char kEraseToStartOfLine[] = ESC "[1K";
constexpr char kEraseLine[] = ESC "[2K";

constexpr char kEraseToEndOfColumn[] = ESC "[J";
constexpr char kEraseToStartOfColumn[] = ESC "[1J";
constexpr char kEraseScreen[] = ESC "[2J";

constexpr char kMoveCursorHome[] = ESC "[H";
constexpr char kSaveCursorPosition[] = ESC "[s";
constexpr char kRestoreCursorPosition[] = ESC "[u";
constexpr char kHideCursor[] = ESC "[?25l";
constexpr char kShowCursor[] = ESC "[?25h";
constexpr char kMoveCursorUp[] = ESC "[A";
constexpr char kMoveCursorDown[] = ESC "[B";
constexpr char kMoveCursorRight[] = ESC "[C";
constexpr char kMoveCursorLeft[] = ESC "[D";

constexpr char kEnableScrolling[] = ESC "[r";
constexpr char kScrollUp[] = ESC "D";
constexpr char kScrollDown[] = ESC "M";

constexpr char kClearCharacterAttributes[] = ESC "[0m";
constexpr char kSetBold[] = ESC "[1m";
constexpr char kSetLowIntensity[] = ESC "[2m";
constexpr char kSetStandout[] = ESC "[3m";
constexpr char kSetUnderline[] = ESC "[4m";
constexpr char kSetBlink[] = ESC "[5m";
constexpr char kSetReverseVideo[] = ESC "[7m";
constexpr char kSetInvisibleText[] = ESC "[8m";

enum class Color { Black, Red, Green, Yellow, Blue, Magenta, Cyan, White };

const char* kForegroundColors[] = {
    "30", "31", "32", "33", "34", "35", "36", "37",
};

const char* kBackgroundColors[] = {
    "40", "41", "42", "43", "44", "45", "46", "47",
};

template <size_t n>
void Put(const char (&message)[n]) {
  write(STDOUT_FILENO, message, n);
}

void Put(const std::string& message) {
  write(STDOUT_FILENO, message.data(), message.size());
}

bool GetSize() {
  struct winsize screen_size;
  int result = ioctl(STDIN_FILENO, TIOCGWINSZ, &screen_size);
  if (result == -1) {
    fprintf(stderr, "error: Unable to get terminal size.\n");
    return false;
  }
  cols = static_cast<size_t>(screen_size.ws_col);
  rows = static_cast<size_t>(screen_size.ws_row);
  return true;
}

void RestoreOriginalTermios() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_original_termios) == -1)
    fprintf(stderr, "error: Failed to restore original terminal attributes.\n");
}

bool EnableRaw() {
  if (!isatty(STDIN_FILENO)) {
    fprintf(stderr, "error: stdin is not a tty.\n");
    return false;
  }
  if (tcgetattr(STDIN_FILENO, &g_original_termios) == -1) {
    fprintf(stderr, "error: Cannot get terminal attributes.\n");
    return false;
  }
  if (atexit(RestoreOriginalTermios) != 0) {
    fprintf(stderr, "error: Cannot register cleaup handler.\n");
    return false;
  }
  struct termios raw = g_original_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    fprintf(stderr, "error: Cannot enable raw terminal input.\n");
    return false;
  }
  return true;
}

bool Init() {
  return term::GetSize() && term::EnableRaw();
}

}  // namespace term

namespace zi {

class ScopedFD {
 public:
  explicit ScopedFD(int fd);
  ~ScopedFD();

  bool is_valid() const { return fd_ != -1; }
  int get() const { return fd_; }

 private:
  int fd_;

  DISALLOW_COPY_AND_ASSIGN(ScopedFD);
};

ScopedFD::ScopedFD(int fd) : fd_(fd) {}

ScopedFD::~ScopedFD() {
  if (is_valid())
    close(fd_);
}

class CommandBuffer {
 public:
  CommandBuffer();
  ~CommandBuffer();

  template <typename T>
  CommandBuffer& operator<<(T&& value) {
    stream_ << value;
    return *this;
  }

  void Write(const char* buffer, size_t length);
  void MoveCursorTo(int x, int y);
  void SetForegroundColor(term::Color color);
  void SetBackgroundColor(term::Color color);
  void Execute();

 private:
  std::ostringstream stream_;

  DISALLOW_COPY_AND_ASSIGN(CommandBuffer);
};

CommandBuffer::CommandBuffer() {}

CommandBuffer::~CommandBuffer() {}

void CommandBuffer::Write(const char* buffer, size_t length) {
  stream_.write(buffer, length);
}

void CommandBuffer::MoveCursorTo(int x, int y) {
  stream_ << ESC "[" << y + 1 << ";" << x + 1 << "H";
}

void CommandBuffer::SetForegroundColor(term::Color color) {
  stream_ << ESC "[" << term::kForegroundColors[static_cast<int>(color)] << "m";
}

void CommandBuffer::SetBackgroundColor(term::Color color) {
  stream_ << ESC "[" << term::kBackgroundColors[static_cast<int>(color)] << "m";
}

void CommandBuffer::Execute() {
  term::Put(stream_.str());
}

class File {
 public:
  File();
  ~File();

  void Clear();
  void Read(const std::string& path);

  void Display(CommandBuffer* commands, int line_index);

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

void File::Display(CommandBuffer* commands, int line_index) {
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
    if (c == 'q')
      return 0;
    Display();
  }
  return 0;
}

void Shell::Display() {
  CommandBuffer commands;
  commands << term::kEraseScreen << term::kMoveCursorHome;
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
  commands << status_ << term::kEraseToEndOfLine;
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
