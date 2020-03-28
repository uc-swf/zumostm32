/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: display.h
 Pfad:  zumolib/inc/zumo

 Erstellt:    20.08.2019
 Autor:       Tobias Ellermeyer
*/

#ifndef ZUMO_DISPLAY_H_
#define ZUMO_DISPLAY_H_

void display_init();
void display_update();
void display_clear();
void display_clear_f();
void display_println(char* msg);
void display_gotoxy(int x, int y);
void display_scroll();
void display_line_f(int x1, int y1, int x2, int y2);



#endif /* ZUMO_DISPLAY_H_ */
