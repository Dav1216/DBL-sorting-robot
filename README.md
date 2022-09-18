This robot is used in adjacency to a moving factory conveyor belt. The goal is for the robot to recognize the presence of the disks coming on the belt, interpret them and decide based on specific patterns chosen by the user whether to accept them or not into forming a stack. An LCD display presents live each type of disk present up to that point in the stack: white, black or green with specfic pictograms. The user presses buttons to select desired pattern.

It works like this: the color sensor must be calibrated first, then the user must choose a pattern, otherwise the display will show "Please choose" and will let all disks go past it, it fills the stack based on the pattern when one is chosen and afterwards, when the stack and the correspondign live view of the stack on the display are full, the display shows "Stack full,empty" in order for somebody to empty the stack so it can start all over again with making the patterns.


The build includes:
- Arduino Uno
- TCS230 color sensor
- IR Break Beam Sensor
- Breadboard
- Buttons
- Servo Motor SG90
- Fischertechnik pieces
- 10K ohm resistors
- Hook-up wires
