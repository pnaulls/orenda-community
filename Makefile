

particle := bin/particle
firmware := firmware/orenda.bin

device := ${DEVICE_ID}

ifeq ($(strip $(device)),)

all flash:
	@echo "DEVICE_ID environment variable not set"
	@exit 1

else

all:
	$(particle) -q compile p1 orenda --saveTo $(firmware)

flash:  $(firmware)
	$(particle) flash $(device) $(firmware)
endif
