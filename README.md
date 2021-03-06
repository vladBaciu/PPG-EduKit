# PPG-EduKit
## An Adjustable Photoplethysmography Evaluation System for Educational Activities

The <b> PPG EduKit </b> is an evaluation platform aiming to acquire the PPG signal from the finger(s) for research and educational purposes. The PPG EduKit gives the user full control and understanding over the different steps needed to acquire,  condition, visualize  and  process  the  raw  PPG  signals  which  are  then  used  to  extract  meaning-ful  physiological  information.  The  modular  approach  of  the  platform  facilitates  the support of different embedded devices such as Arduino and PSoC 6 platform, to perform digital signal processing operations needed for the multiple PPG applications.  The components of the PPG EduKit are grouped in two main blocks:  the analog front-end and the digital back-end.


![EduKit_Picture](https://user-images.githubusercontent.com/24388880/150654604-58d1e7ed-1703-4d63-a5f6-fa29669c30ae.PNG)


The PPG EduKit platform can be used as a shield for any Arduino board as long as the microcontroller runs with a logic level voltage of 3V3. <b> Do not use the PPG EduKit with microcontrollers that have a logic level voltage of 5V, like the Arduino UNO. </b> The platform can be also used with CY8CPROTO-063-BLE Prototyping Kit (Cypress/Infineon) using the bridge adaptor.

# Publication
Angel Solé Morillo, Joan Lambert Cause, Vlad-Eusebiu Baciu,  Bruno da Silva, Juan Carlos Garcia-Naranjo and Johan Stiens, [ PPGEduKit: An Adjustable Photoplethysmography Evaluation System for Educational Activities](https://www.mdpi.com/1424-8220/22/4/1389), In Sensors MDPI, 2022

## Citation
If you find any of the ideas/codes useful for your research, please cite our paper:

	@article{morillo2022ppgedukit,
  		title={PPG EduKit: An Adjustable Photoplethysmography Evaluation System for Educational Activities},
  		author={Sole Morillo, Angel, Lambert Cause, Joan, Baciu, Vlad-Eusebiu, da Silva, Bruno, Garcia-Naranjo, Juan Carlos and Stiens, Johan },
 		  journal={Sensors MDPI},
  		year={2022}
	}
  
# Repository structure

<a href="Arduino/"> Arduino directory</a> : includes the PPG EduKit Arduino library and a number of guiding labs using the library package.

<a href="PSoC/"> PSoC directory</a> : includes the PSoC libraries and a number of guiding labs using the library package.

<a href="GUI_Application/"> GUI_Application directory</a> : contains the PPG EduKit GUI application. There are a number of two applications implemented that allows the user to visualize the PPG signals.

<a href="Others/"> Others directory</a> : contains platform related documents and datasheets.

# 

![IMG_6443__online-video-cutter_com__AdobeCreativeCloudExpress](https://user-images.githubusercontent.com/24388880/150688649-eb10124e-2acd-44b6-abc6-9c9da6b65890.gif) ![logo](https://user-images.githubusercontent.com/24388880/150688874-47fda6dc-76bc-4e11-b990-43bc88ff3941.jpg)

## Requirements

For the GUI Application you need any Python IDE, but we suggest using <b> Anaconda </b>. [Anaconda](https://www.anaconda.com/products/individual) is a distribution of the Python that aims to simplify package management and deployment. 

Using Anaconda, install <b> Spyder version 4.1.5 </b>, a free integrated development environment (IDE) that is included with Anaconda.

In order to use the GUI Application, please install the following packages: 
```bash
pip install pyqtgraph==0.12.3 --user
pip install numpy==1.19.5 --user
pip install PyQt5==5.15.6 --user
pip install pyserial==3.5 --user
```


## Contact:

[Email](mailto:vlad-eusebiu.baciu@vub.be;angelsm@etrovub.be;jlambert@etrovub.be;bdasilva@etrovub.be?subject=[GitHub]%20PPG%20EduKit%20contact)

