# Picopad TFT Display Emulator

This project provides an emulator for the TFT display of Picopad handheld devices. It's designed to assist in the
prototyping and development of applications for Picopad handhelds. With this tool, you can easily prepare and test your
display code using the API of the Picopad SDK.

Code of this emulator is based on the original Picopad SDK source code.

## Features

- Display emulation: The emulator accurately represents the display characteristics of Picopad handhelds, allowing you
  to see how your application will look on the actual device.
- Keyboard input: Simulate changes in device state with keypress events (e.g., toggle sensor connection with the 'C'
  key).
- Easy prototyping: Modify the provided main file to draw different UI components on the display.

## Getting Started

### Prerequisites

This emulator is based on SFML (Simple and Fast Multimedia Library) and the Picopad SDK. Ensure you have SFML on your
system.

Certainly. Here's how you can install SFML on macOS and use it in your project with GNU Make.

### Installing SFML on macOS

1. Install Homebrew if you don't have it installed already. You can install it by pasting the following command in the
   terminal:

    ```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    ```

2. Once Homebrew is installed, you can install SFML by running the following command:

    ```bash
    brew install sfml
    ```

This will download and install the latest version of SFML on your system.

3. Update the following lines in your `Makefile` file:

```make
# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Target executable
TARGET = tft-emulator

# Source files
SRCS = main.cpp

# SFML includes and libraries (adjust these paths to your environment)
SFML_LIB_DIR = /usr/local/Cellar/sfml/2.6.1/lib
SFML_INCLUDE_DIR = /usr/local/Cellar/sfml/2.6.1/include/SFML

# Link SFML libraries (Make sure to include the window and system components)
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# Compiler flags and linker flags for SFML
INCLUDES = -I$(SFML_INCLUDE_DIR)
LDFLAGS = -L$(SFML_LIB_DIR) $(SFML_LIBS)

# Default target
all: $(TARGET)

# Build target executable
$(TARGET): $(SRCS)
$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $(TARGET) $(LDFLAGS)

# Clean up
clean:
rm -f $(TARGET)
```

4. In the terminal, navigate to your project directory and build your project with CMake:

    ```bash
    mkdir build
    cd build
    make all ..
    ```

Your project should now be able to use SFML.

## Running the Emulator

1. Clone this repository to your local machine.
2. Navigate to the project's directory.
3. Update Makefile
4. Compile the source code with your GNU Make.

## Usage

Modify the `main.cpp` file to create the UI of your application using the PicoLibSDK DRAW API.

## Links

- Picopad from Pajenicko e-shop (https://pajenicko.cz/picopad-pro-open-source-herni-konzole-osazeny)
- Picopad SDK (https://github.com/Pajenicko/Picopad)

