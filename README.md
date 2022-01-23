# PPG-EduKit
## An Adjustable Photoplethysmography Evaluation System for Educational Activities

The <b> PPG EduKit </b> is an evaluation platform aiming to acquire the PPG signal from the finger(s) for research and educational purposes. The PPG EduKit gives the user full control and understanding over the different steps needed to acquire,  condition, visualize  and  process  the  raw  PPG  signals  which  are  then  used  to  extract  meaning-ful  physiological  information.  The  modular  approach  of  the  platform  facilitates  the support of different embedded devices such as Arduino and PSOC6 platform, to perform digital signal processing operations needed for the multiple PPG applications.  The components of the PPG EduKit are grouped in two main blocks:  the analog front-end and the digital back-end.


![EduKit_Picture](https://user-images.githubusercontent.com/24388880/150654604-58d1e7ed-1703-4d63-a5f6-fa29669c30ae.PNG)


The PPG EduKit platform can be used as a shield for any Arduino board as long as the microcontroller runs with a logic level voltage of 3V3. <b> Do not use the PPG EduKit with microcontrollers that have a logic level voltage of 5V, like the Arduino UNO. </b> The platform can be also used with CY8CPROTO-063-BLE Prototyping Kit (Cypress/Infineon) using the bridge adaptor provided (the Gerber files can be found <a href="Others/Adapter_Board">here</a>).

# Repository structure

<a href="Arduino/"> Arduino directory</a> : includes the PPG EduKit Arduino library and a number of guiding labs using the library package.

<a href="PSOC/"> PSOC directory</a> : includes the PSOC libraries and a number of guiding labs using the library package.

<a href="GUI_Application/"> GUI_Application directory</a> : TBA

<a href="Others/"> Others directory</a> : TBA




