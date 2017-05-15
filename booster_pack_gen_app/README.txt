------------------
Booster Pack Generator
------------------

A lightweight app that takes card images from folders and spits out print-and-play booster pack sheets.
version 2.0

Made by Andy Wallace.
AndyMakes.com
@andy_makes
andy@andymakes.com

Created using openFrameworks.

The code for this project can be found here: https://github.com/andymasteroffish/BoosterPackGenerator


------------------
What is it?
------------------

This app will create ready to print booster packs by drawing images from folders and randomly putting them into sets.

Images must be .jpg .gif or .png. Any files not of this type will be skipped.

There is a settings.xml file that the app will look for that describes where the card images are and with what frequency they should appear (among other things).
This file must be in the same folder as the app.

You can edit settings.xml by opening it up with any text editting program.


------------------
Settings XML
------------------

The settings.xml file is how you determine how the booster packs will be made & printed. It is broken up into three sections:
-general settings, options about how many packs to print and how to print them
-image sources, any number of folders that you want to pull images from (most representing different rarities)
-rarity tiers, describing which folder(s) each tier should draw from at what frequency

It is an XML file and so must follow XML formatting. Some example fields are given for img_sources and rarity_tiers. These are just to show what the formatting looks like and can be altered or removed entirely.


------------------
General Settings
------------------

The first part of settings.xml is a set of fields determining how the packs will print. Fields should not be added or removed from this section and true/false values must be capitalized ("TRUE" will work but "True" or "true" will not).

Here is what each field does:

NUM_PACKS : How many packs to print.

ALLOW_DUPLICATES : If set to “TRUE” a single pack can contain duplicates of the same card. If “FALSE”, each booster pack will contain only one of each card (assuming there are enough images in the source folder to do this).

OUTPUT_FOLDER : The path to the folder on your computer that the resulting printouts should go to. If the folder does not exist, the app will create it.

SHOW_PACK_NUMBERS : If set to "TRUE" a small box will appear at the bottom left corner showing the number of this pack. Useful because packs may be spread across several sheets.

EDGE_PADDING : The amount of white space (in pixels) around the edge of each sheet.

CARD_PADDING : The amount of white space (in pixels) between each card.

CLOSE_WHEN_FINISHED : If set to "TRUE" the app will exit automatically once it is done creating the sheets.

PRINT_ALL_CARDS_IN_ORDER: If set to “TRUE”, overrides NUM_PACKS, ALLOW_DUPLICATES & SHOW_PACK_NUMBERS. Instead, this will just print out one of every card in the three folders. No randomness. Use this to print out a set instead of booster packs.

------------------
Image Sources
------------------

Inside the <img_sources> tag any number of folders can be marked for use in the app. Each folder should contain a set of card images to be used in making the pack. Generally speaking, they should be grouped by rarity level.

Each folder tag has as its value the file location on your computer of the folder that holds that set of images. Some text editors will allow you to drag a folder on your computer into the text and automaticly paste the file location. 

You can use an absolute or relative path (relative to the data folder of the application). For example, a using a value of "../common" would look for a folder named "common" located next to the applicaiton and data folder. An absolute path like "/Users/your_user_name/Desktop/common" would look for a folder named "common" on the desktop of a user named "your_user_name."

Each folder tag needs and id name (contained inside the folder tag as the "idName" attribute) that will be used to identify it for rarity tiers.

These image sources are seperate from rarity tiers so that a given tier could potentialy pull an image from multiple folders based on random chance. For instance, the rare slot of a pack may pull from the "rare" folder most of the time, but pull from the "mythic" folder a small percentage of the time.


------------------
Rarity Tiers
------------------

Rarity tiers describe the card-per-card makeup of a single booster back. Each rarity tier will show up a set number of times per pack (defined as <count>) and will draw a card image from one of the folders ascociated with that tier.

Within the <rarity_tier> tag in settings.xml you can have any number of <tier> tags.

The most common setup is that a rarity tier will be ascociated with a single folder and will have a 100% chance of drawing an image from that folder.

Each rarity tier can have an optional name attribute inside the <tier>. This name is only used in error and warning reporting so it can be convenient to include but is not strictly necessary. 

All rarity tiers must contain a <count> tag that the tells the app how many cards to include at this tier.

A rarity tier can draw from any number of folders defined in the <img_source> tag and must include at least one. A simple rarity tier might be defined like this:

	<tier name = "common slot">
		<count>4</count>
		<folder>common</folder>
	</tier>

This will make sure that each booster pack includes 4 card images drawn from the folder with the idName "common."

A more complex tier might look like this:

	<tier name = "promo slot">
		<count>1</count>
		<folder chance=80>rare</folder>
		<folder chance=15>mythic</folder>
		<folder chance=5>promo</folder>
	</tier>

There will be 1 card in each pack at this rarity, but that card image might come from the "rare", "mythic", or "promo" folder depending on a random outcome defined by the "chance" attribute of each folder. There is an 80%, 15% and 5% chance for each folder to be used respectively.

A rarity tier like this can be used to account for rarity levels that do not occur in every pack, or to occasionaly let a card at one rarity level be replaced with one from a different source folder.

As you can see in the more straight forward "commons" example, if a folder does not have a set chance attribute, it will default to 100. If the chance values of the folders in a given tier do not add up to 100, the application will account for that.

