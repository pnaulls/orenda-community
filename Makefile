

particle := bin/particle
firmware := firmware/orenda.bin

device := 350054001651363036373538

all:
	$(particle) compile p1 orenda --saveTo $(firmware)

flash:
	$(particle) flash $(device) $(firmware)
