# commanddeck

Command Deck is an open-source, tech-savvy alternative to Stream Deck-like devices, allowing you to assign several actions to a single key on a 4x4 keypad.

It can be used with software like Emacs, Vim, OBS and basically anything that accepts keyboard commands.

Mainly, such devices are useful for streamer to perform quick reactions to content, but many workflows are possible. I personally use it for coding.

## Features

- Supports virtually unlimited modes (i.e. sets of [keypad key -> action] mappings), you are only limited by NodeMCU's 4MB flash
- Basically all keyboard keys are supported, including modifiers
- Macros can become as complex as you like (check modes.h)
- Programmable delays
- Fine-control of key press and release
- Over-the-Air updates
- [Blynk](https://blynk.io/) bi-redctional integration

## Images

![Picture](img/picture.jpg?raw=true "Picture")
![Blynk](img/blynk.jpg?raw=true "Blynk")

## Parts

It consists of the following parts:

- 1x Arduino Leonardo Pro Micro, used to emulate a HID Keyboard
- 1x NodeMCU, where the brain of the project lives, alongside the actions you assign
- 1x KY040 Rotary Encoder, to switch between modes
- 1x 4x4 Matrix keypad
- 1x 0.91'' OLED screen
- 1x regular LED
- 1x 3D-printed case (STL on the stl/ folder)

## Assembly

TO-DO

## Usage

- Create your actions on modes.h, follow the examples there to get started.

- Edit secrets.h accordingly

- Move platformio_example.ini to platformio.ini, but don't uncomment the last lines yet

- Connect your NodeMCU and upload the sketch using `platformio run -t upload`, hopefully this is the last time you will need the NodeMCU connected on the USB

- Connect your Arduino Leonardo Pro Micro and follow the instructions on [commanddeck-leonardo](https://github.com/kamushadenes/commanddeck_leonardo)

Next time you want to upload something to the NodeMCU, uncomment the lines on platformio.ini, making sure to replace the OTA authentication password with the one which hash you put on secrets.h.

You'll hardly need to upload something to the Arduino again, but it should be a no brainer, specially given it will likely be connected all the time.

- Follow the assembly instructions

- Connect an USB cable from your computer to the Arduino Leonardo

- Enjoy

## Future Roadmap
- Web Interface to program the actions on the NodeMCU
- ???
