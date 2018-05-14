

particle := bin/particle
firmware := firmware/orenda.bin

device := ${DEVICE_ID}

all:
	$(particle) -q compile p1 orenda --saveTo $(firmware)

flash:  $(firmware)
ifeq ($(strip $(device)),)
	@echo "DEVICE_ID environment variable not set"
else
	$(particle) flash $(device) $(firmware)
endif