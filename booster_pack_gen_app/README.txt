------------------
BoosterPackGenerator
------------------

A lightweight app that takes card images from folders and spits out print and play booster pack sheets.
version 1.0

Made by Andy Wallace. AndyMakes.com
@andy_makes
andy@andymakes.com

Created using openFrameworks.

The code for this project can be found here: https://github.com/andymasteroffish/BoosterPackGenerator


------------------
What is it?
------------------

This app will create ready to print booster packs by drawing images from three folders (common, uncommon, rare) and randomly putting them into sets.

The card images can be of any size but they must all be the same size. Images must be .jpg .gif or .png.

There is a settings.xml file that the app will look for that describes where the card images are and with what frequency they should appear (among other things).
This file must be in the same folder as the app.

You can edit settings.xml by opening it up with any text editting program.


------------------
Settings XML
------------------

Here is what each field does:

NUM_PACKS : How many packs to print

NUM_COMMON : How many common cards per pack
NUM_UNCOMMON : How many uncommon cards per pack
NUM_RARE : How many rare cards per pack

COMMON_SOURCE_FOLDER : path to the folder on your computer that contains the common card images
UNCOMMON_SOURCE_FOLDER : path to the folder on your computer that contains the uncommon card images
RARE_SOURCE_FOLDER : path to the folder on your computer that contains the rare card images

OUTPUT_FOLDER : The path to the folder on your computer that the resulting printouts should go to. If the folder does not exist, the app will create it.

SHOW_PACK_NUMBERS : If set to "TRUE" a small box will appear at the bottom left corner showing the number of this pack. Useful because packs may be spread across several sheets.

CLOSE_WHEN_FINISHED : If set to "TRUE" the app will exit automatically once it is done creating the sheets

EDGE_PADDING : The amount of white space (in pixels) around the edge of each sheet
CARD_PADDING : The amount of white space (in pixels) between each card

PRINT_ALL_CARDS_IN_ORDER: If set to “TRUE”, overrides the first 4 settings (num packs, number of each card type) as well as the SHOW_PACK_NUMBERS. Instead, this will just print out one of every card in the three folders. No randomness. Use this to print out a set instead of booster packs.