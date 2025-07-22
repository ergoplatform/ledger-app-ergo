from ragger.conftest import configuration
import random
import os

###########################
### CONFIGURATION START ###
###########################


__location__ = os.path.realpath(
    os.path.join(os.getcwd(), os.path.dirname(__file__)))
with open(os.path.join(__location__, "seed.txt"), "r") as file:
    MNEMONIC = file.read()

configuration.OPTIONAL.CUSTOM_SEED = MNEMONIC

#########################
### CONFIGURATION END ###
#########################

# Pull all features from the base ragger conftest using the overridden configuration
pytest_plugins = ("ragger.conftest.base_conftest", )

random.seed(0)