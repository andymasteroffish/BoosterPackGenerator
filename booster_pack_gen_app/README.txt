------------------
Booster Pack Generator
------------------

A lightweight app that takes card images from folders and spits out print-and-play booster pack sheets.
version 2.0

Note: This tool is for printing full booster packs and is mostly useful for folks making full fan sets or running proxy booster drafts. If you just want to print proxies of a few cards, I have a much more user friendly tool here: https://andymakes.itch.io/tcg-proxy-generator


Made by Andy Wallace.
AndyMakes.com
@andy_makes
andy@andymakes.com


------------------
What is it?
------------------

This app will create ready-to-print booster packs by drawing card images from folders and randomly putting them into sets.

Images must be .jpg .gif or .png. Any files not of this type will be skipped.

There is a settings.xml file that the app will look for that describes where the card images are, and with what frequency they should appear (among other things). This is how you set the parameters for the app to use when it builds the packs.
This file must be in the same folder as the app.

You can edit settings.xml by opening it with any text editing program. Sublime is a nice one: https://www.sublimetext.com/3


------------------
Settings XML
------------------

The settings.xml file is how you determine how the booster packs will be made & printed. It is broken up into three sections:
-General Settings, options about how many packs to print and how to print them
-Image Sources, any number of folders that you want to pull images from (most representing different rarities)
-Rarity Tiers, describing which folder(s) each tier should draw from at what frequency

It is an XML file and so must follow XML formatting. Some example fields are given for img_sources and rarity_tiers. These are just to show what the formatting looks like and can be altered.

You may want to open up that file now and take a look at it.


------------------
General Settings
------------------

The first part of settings.xml is a set of fields determining how the packs will print. Fields should not be added or removed from this section, and true/false values must be capitalized ("TRUE" will work but "True" or "true" will not).

Here is what each field does:

NUM_PACKS : How many packs to print.

ALLOW_DUPLICATES : If set to “TRUE” a single pack can contain duplicates of the same card. If “FALSE”, each booster pack will contain only one of each card (assuming there are enough images in the source folder to do this).

OUTPUT_FOLDER : The path to the folder on your computer that the resulting printouts should go to. If the folder does not exist, the app will create it.

EXPORT_PDF : If set to "TRUE" a pdf of all of the pages will be saved in the output folder.

EXPORT_PNG : If set to "TRUE" each page will be saved as an individual png file in the output folder
(EXPORT_PDF & EXPORT_PNG can both be set to true).

SHOW_PACK_NUMBERS : If set to "TRUE" a small box will appear at the bottom left corner showing the number of this pack. Useful because packs may be spread across several sheets.

EDGE_PADDING : The amount of white space (in pixels) around the edge of each sheet.

CARD_PADDING : The amount of white space (in pixels) between each card.

CLOSE_WHEN_FINISHED : If set to "TRUE" the app will exit automatically once it is done creating the sheets.

PRINT_ALL_CARDS_IN_ORDER: If set to “TRUE”, overrides NUM_PACKS, ALLOW_DUPLICATES & SHOW_PACK_NUMBERS. Instead, this will just print out one of every card in the three folders. No randomness. Use this to print out a set instead of booster packs.


------------------
Image Sources
------------------

Inside the <img_sources> tag, any number of folders can be marked for use in the app. Each folder should contain a set of card images to be used in making the booster packs. Generally speaking, they should be grouped by rarity level.

Each folder tag has as its value the file location on your computer of the folder that holds that set of images. Some text editors will allow you to drag a folder on your computer into the text editor and automatically paste the file location.

You can use an absolute or relative path (relative to the data folder of the application). For example, a using a value of "../common" would look for a folder named "common" located next to the application and data folder. An absolute path like "/Users/your_user_name/Desktop/common" would look for a folder named "common" on the desktop of a user named "your_user_name."

Each folder tag needs an id name (contained inside the folder tag as the "idName" attribute) that will be used to identify it for rarity tiers.

These image sources are separate from rarity tiers so that a given tier could potentially pull an image from multiple folders based on random chance. For instance, the rare slot of a pack may pull from the "rare" folder most of the time, but pull from the "mythic" folder a small percentage of the time.


------------------
Rarity Tiers
------------------

Rarity tiers describe the card-per-card makeup of a single booster back. Each rarity tier will show up a set number of times per pack (defined as <count>) and will draw a card image from one of the folders associated with that tier.

Within the <rarity_tier> tag in settings.xml you can have any number of <tier> tags.

The most common setup is that a rarity tier will be associated with a single folder and will have a 100% chance of drawing an image from that folder.

Each rarity tier can have an optional name attribute inside the <tier>. This name is only used in error and warning reporting, so it can be convenient to include, but is not strictly necessary. 

All rarity tiers must contain a <count> tag that the tells the app how many cards to include at this tier.

A rarity tier can draw from any number of folders defined in the <img_source> tag, but must include at least one. A simple rarity tier might be defined like this:

	<tier name = "common slot">
		<count>5</count>
		<folder>common</folder>
	</tier>

This will make sure that each booster pack includes 5 card images drawn from the folder with the idName "common."

A more complex tier might look like this:

	<tier name = "promo slot">
		<count>1</count>
		<folder chance=80>rare</folder>
		<folder chance=15>mythic</folder>
		<folder chance=5>promo</folder>
	</tier>

There will be 1 card in each pack at this rarity, but that card image might come from the "rare", "mythic", or "promo" folder, depending on a random outcome defined by the "chance" attribute of each folder. There is an 80%, 15% and 5% chance for each folder to be used, respectively.

A rarity tier like this can be used to account for rarity levels that do not occur in every pack, or to occasionally let a card at one rarity level be replaced with one from a different source folder.

As you can see in the more straightforward "commons" example, if a folder does not have a set chance attribute, it will default to 100.

------------------
Issues / Concerns
------------------

If something is not working or if there is some feature you'd like to see, hit me up at andy@andymakes.com or on twitter at @andy_makes. I have a full-time job other than dev, and I tend to have a lot of game projects going at once, so no promises that I will be able to fix/change things in a timely fashion or at all, but as long as you don't mind that, I'd love to hear from you! Most of the updates in this version happened because @CPL_AKIBA reached out to me about 6 months ago!

You can check out my other games on Itch: itch.io/andymakes

This tool is free. You can share it, although I’d like it if you kept this readme with it.


If this tool is useful to you (or if it isn’t), please donate to one of these amazing organizations:

Lambda Legal
www.lambdalegal.org

ACLU
www.aclu.org

Sylvia Rivera Law Project
srlp.org

Planned Parenthood
www.plannedparenthood.org

Black Lives Matter
blacklivesmatter.com


This tool was written in openFrameworks 0.9.3
The source code for this project can be viewed here:
https://github.com/andymasteroffish/BoosterPackGenerator