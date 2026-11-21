Filtry LADSPA, w zamyśle, pisane do współpracy z Audacity oraz LMMS. Na początek, jedna wtyczka.

### przem-low-shelf

- Wtyczka przem-low-shelf, implementuje cyfrowy filtr typu Low-Shelf (półkowy dolnoprzepustowy), który wzmacnia sygnał poniżej określonej częstotliwości (np. 100 Hz). Wtyczka LADSPA operuje na surowych próbkach dźwięku. Najlepszym i standardowym rozwiązaniem w DSP (cyfrowym przetwarzaniu sygnałów) jest filtr bikwadratowy (Biquad) zaprojektowany na podstawie wzorów Roberta Bristow-Johnsona. Jest to filtr stereofoniczny. Parametry 'gain' oraz 'frequency' są ze sobą powiązane, jeśli chcesz uzyskać jakikolwiek efekt. Powinieneś zwiększyć/zmienić oba parametry na raz !

