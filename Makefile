.PHONY: all cleansub

all:
	$(MAKE) -f make_eu.mk
	$(MAKE) -f make_us.mk
	$(MAKE) -f make_jp.mk

cleansub:
	rm -f *.o
	$(MAKE) -f make_eu.mk cleansub
	$(MAKE) -f make_us.mk cleansub
	$(MAKE) -f make_jp.mk cleansub