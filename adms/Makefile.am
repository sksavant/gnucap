all: gnucap_0.xml

gnucap_0.xml: gnucap.xml
	./split.py

%.xml: %.xml.in 
	convert_symb/convert $< -o $@ -addxml

convert_symb/convert:
	cd convert_symb/ && make

clean:
	rm -f *.cc *.h *.o *~ *.c gnucap.xml *.hxx *.so gnucap_*.xml

