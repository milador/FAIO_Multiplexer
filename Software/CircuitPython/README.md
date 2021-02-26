# FAIO-Multiplexer CircuitPython software setup

  1. Download latest version of [CircuitPython for Feather M0 Express UF2](https://circuitpython.org/board/feather_m0_express/)
  2. Plug your Feather M0 Express into your computer using a known-good USB cable
  3. Double-click the small RST (reset) button, and you will see the NeoPixel RGB LED turn green
  4. You will see a new disk drive appear called FEATHERBOOT under Device and Drivers 
  5. Drag the adafruit_circuitpython_etc.uf2 file to FEATHERBOOT
  6. The red LED will flash. Then, the FEATHERBOOT drive will disappear and a new disk drive called CIRCUITPY will appear
  7. Replace the code.py under CIRCUITPY drive with FAIO Multiplexer USB [code.py](https://raw.githubusercontent.com/milador/FAIO_Multiplexer/main/Software/CircuitPython/FAIO_Multiplexer_USB_Software/code.py)
  8. Drag and drop required libraries under CIRCUITPY drive in lib folder. [Required libraries](https://github.com/milador/FAIO_Multiplexer/tree/main/Software/CircuitPython/FAIO_Multiplexer_USB_Software/lib)
  9. The led on Enabled-Controller-Mini should turn to teal color
