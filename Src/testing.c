/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: testing.c
 Pfad:  src

 Erstellt:    08.11.2019
 Autor:       Tobias Ellermeyer

 28.03.2020 (TE):
 	 test_bt() entfernt, da nicht mehr benoetigt
*/


#include <stdio.h>
#include "main.h"
#include <zumo/gpio_sx1508.h>
#include <zumo/display.h>
#include <zumo/motors.h>
#include <zumo/distance.h>
#include <zumo/bno055.h>
#include <zumo/bluetooth.h>
#include <math.h>
#include "testing.h"

/**
 * @file testing.c
 *
 * @brief Testfunktionen zum Überprüfen der Hardware
 *
 *
 */


#define BTN_OK 0x01    ///< Merker für OK Button (Binary)
#define BTN_DN 0x02	   ///< Merker für Down Button (Binary)
#define BTN_UP 0x04    ///< Merker für Up Button (Binary)
#define BTN_USR 0x08   ///< Merker für User Button (Binary)
#define BTN_WAIT 1	   ///< in buttonpressed warten, bis Taste gedrückt
#define BTN_NOWAIT 0   ///< in buttonpressed nicht warten, nur abfragen

extern I2C_HandleTypeDef hi2c1;		///< Handle für I2C Bus, bei scan_i2c benötigz

static uint8_t buttonpressed(uint8_t wait);
static void test_motors();
static void test_bt();
static void test_buzzer();
static void test_leds();
static void test_distance();
static void testing_draw_body();
static void testing_draw_dist(int x, int y, int intensity, int rot);
static void test_bno055();
static void test_btname();


/***************************************************************************/
/**
 * @brief Das Menue für die Testroutinen ausgeben
 *
 * @param none
 * @return none
 *
 */
void test_menu(uint8_t preselect)
{
	int s=1;
	int smax=6;
	int key;

	while(1)
	{
		if (!preselect)
		{
			// Menue ausgeben
			display_clear();
<<<<<<< HEAD
			display_println("|  HW-TESTS   |\n"
							"'-------------'\n"
							"  LEDs\n"
							"  Motor/Encode\n"
							"  Buzzer\n"
							"  BNO055\n"
							"  ToF-Sensoren\n"
							"  Bluetooth Name");
=======
			display_println("|  HW-TESTS   |");
			display_println("'-------------'");
			display_println("  LEDs");
			display_println("  Motor/Encode");
			display_println("  Buzzer");
			display_println("  BNO055");
			display_println("  ToF-Sensoren");
			display_println("  Bluetooth Name");
>>>>>>> refs/remotes/origin/master

			// "Wandernder" Pfeil, bei BTN_OK Schleife verlassen
			while (1)
			{
				// Pfeil zeichnen
				display_gotoxy(0,s+1);
				display_println(">");

				// Auf Butten warten
				key = buttonpressed(BTN_WAIT);

				// Alte Pfeilposition löschen
				display_gotoxy(0,s+1);
				display_println(" ");

				// Pfeil hoch mit Anschlag oben
				if ( (key&BTN_UP) && (s>1))
				{
					s--;
				}
				// Pfeil runter mit Anschlag unten
				if ((key&BTN_DN) && (s<smax))
				{
					s++;
				}
				// Auswahl? -->Schleife verlassen
				if (key&BTN_OK)
				{
					break;
				}
			}
		}
		else
		{
			// Hinweis
			display_clear();
			display_println("PRESELECT MODE");
			HAL_Delay(1000);
		}
		// Preselect beenden
		if ( (preselect > 0) && (preselect <6) )
		{
			s = preselect;
			preselect = 0;
		}
		// Auswahl-Schleife wurde verlassen; entsprechende Aktion starten
		switch(s)
		{
		case 1:	test_leds();
				break;
		case 2:	test_motors();
				break;
		case 3:	test_buzzer();
				break;
		case 4:	test_bno055();
				break;
		case 5:	test_distance();
				break;
		case 6:	test_btname();
				break;
		default: break;
		}
	}
}

/***************************************************************************/
/**
 * @brief Die Motoren und Encoder testen (links/rechts auf der Stelle)
 *
 * @param none
 * @return none
 *
 */
static void test_motors()
{
	int pwm=0;
	int pwmdir=10;
	char buf[20];
	int i;
	uint8_t done=0;

	display_clear();
	display_println("|  MOTOR-TEST  |\n"
					"'--------------'\n"
					"Ende mit <USR>");

	while (!done)
	{
		pwm+=pwmdir;
		if (pwm>=100) pwmdir=-10;
		if (pwm<=-100) pwmdir=10;
		motors_set_left_pwm(pwm);
		motors_set_right_pwm(pwm);

		// Encoder-Werte ausgeben
		sprintf(buf, "ENC: %8li  %8li",motors_encoder_left(), motors_encoder_right());
		display_gotoxy(0,5);
		display_println(buf);

		// Warten, während dessen aber Taster abfragen...
		for(i=0;i<20;i++)
		{
			if (buttonpressed(BTN_NOWAIT) & BTN_USR)
			{
				done=1;
				break;
			}
			HAL_Delay(5);
		}

	}
	// Motoren wieder anhalten
	motors_stop();
}

/***************************************************************************/
/**
 *  @brief Alle aktiven Komponenten am I2C-Bus listen; ausgabe über USB/seriell
 *
 *	@param none
 *	@return none
 */
void scan_i2c()
{
	uint8_t status;
	uint8_t i;

	for(i=1;i<128;i++)
	{
		status = HAL_I2C_IsDeviceReady(&hi2c1, i<<1,1,100);
		if (!status)
			printf(" %02x  ",i);
	}
	printf("\r\n");
}


/***************************************************************************/
/**
 * @brief Summer testen; z.Zt. noch sehr einfach
 *
 * @param none
 * @return none
 */
static void test_buzzer()
{
	int i;

	display_clear();
	display_println("|  Buzzer Test  |\n"
					"'---------------'\n"
					"(2 Toene)");

	for(i=0;i<100;i++)
	{
		HAL_GPIO_WritePin(BUZ_PWM_GPIO_Port, BUZ_PWM_Pin,GPIO_PIN_SET);
		HAL_Delay(2);
		HAL_GPIO_WritePin(BUZ_PWM_GPIO_Port, BUZ_PWM_Pin,GPIO_PIN_RESET);
		HAL_Delay(2);
	}
	for(i=0;i<100;i++)
	{
		HAL_GPIO_WritePin(BUZ_PWM_GPIO_Port, BUZ_PWM_Pin,GPIO_PIN_SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(BUZ_PWM_GPIO_Port, BUZ_PWM_Pin,GPIO_PIN_RESET);
		HAL_Delay(1);
	}

}


/***************************************************************************/
/**
 * @brief LEDs testen
 *
 * @param none
 * @return none
 *
 */
static void test_leds()
{
	int led_no =0;
	uint8_t btns;
	char buf[15];
	display_clear();
	display_println("|  LED Test  |\n"
					"'------------'\n"
					"<UP>  LED weiter\n"
					"<USR> zurück");


	// Schleife
	while(led_no <100)	// led_no >= 100 : Schleife verlassen
	{

		switch(led_no)
		{
		case 0:
			HAL_GPIO_WritePin(LED_BACK_L_GPIO_Port, LED_BACK_L_Pin,GPIO_PIN_SET);
			sprintf(buf,"Hinten Links ");
			break;
		case 1:
			HAL_GPIO_WritePin(LED_BACK_R_GPIO_Port, LED_BACK_R_Pin,GPIO_PIN_SET);
			sprintf(buf,"Hinten Rechts");
			break;
		case 2:
			HAL_GPIO_WritePin(LED_FRONT_GPIO_Port, LED_FRONT_Pin,GPIO_PIN_SET);
			sprintf(buf,"Vorne        ");
			break;
		case 3:
			HAL_GPIO_WritePin(LED_USR0_GPIO_Port, LED_USR0_Pin,GPIO_PIN_SET);
			sprintf(buf,"Display USR0 ");
			break;
		case 4:
			HAL_GPIO_WritePin(LED_USR1_GPIO_Port, LED_USR1_Pin,GPIO_PIN_SET);
			sprintf(buf,"Display USR1 ");
			break;
		default: break;
		}

		// Text ausgeben
		display_gotoxy(0,5);
		display_println(buf);

		// Auf Button warten
		while(1)
		{
			btns = buttonpressed(BTN_WAIT);
			if (btns&BTN_USR)
			{
				led_no = 111;	// Scheife verlassen
				break;
			}
			if (btns&BTN_UP)
			{
				led_no = (led_no+1)%5;
				break;
			}
		}
		// Alle LEDs aus
		HAL_GPIO_WritePin(LED_BACK_L_GPIO_Port, LED_BACK_L_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_BACK_R_GPIO_Port, LED_BACK_R_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_FRONT_GPIO_Port, LED_FRONT_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_USR0_GPIO_Port, LED_USR0_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_USR1_GPIO_Port, LED_USR1_Pin,GPIO_PIN_RESET);
	}
}



/**
 * @brief Flankenerkennung Buttons
 *
 * @param wait
 * 		BTN_NOWAIT -> Nicht warten, 0 zurückgeben wenn kein Taster gedrückt
 *      BTN_WAIT   -> Warten, bis ein Taster gedrückt wurde
 *
 * @return
 *      0 -> Kein Taster
 *      BTN_UP, BTN_DN, BTN_OK, BTN_USR (Oder-Verknüpfung)
 */
static uint8_t buttonpressed(uint8_t wait)
{
	static uint8_t mem_up=0;
	static uint8_t mem_dn=0;
	static uint8_t mem_ok=0;
	static uint8_t mem_usr=0;
	uint8_t ret_code;

	do
	{
		// Buttons einlesen
		mem_up  = (mem_up<<1)  | (HAL_GPIO_ReadPin(BTN_UP_GPIO_Port,BTN_UP_Pin)==0);
		mem_dn  = (mem_dn<<1)  | (HAL_GPIO_ReadPin(BTN_DN_GPIO_Port,BTN_DN_Pin)==0);
		mem_ok  = (mem_ok<<1)  | (HAL_GPIO_ReadPin(BTN_OK_GPIO_Port,BTN_OK_Pin)==0);
		mem_usr = (mem_usr<<1) | (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port,BTN_USR_Pin)==0);

		// Nur 4 Bits berücksichtigen
		mem_up  &= 0x0F;
		mem_dn  &= 0x0F;
		mem_ok  &= 0x0F;
		mem_usr &= 0x0F;

		// Auswerten
		ret_code=0;

		if (mem_up == 0x07) ret_code |= BTN_UP;
		if (mem_dn == 0x07) ret_code |= BTN_DN;
		if (mem_ok == 0x07) ret_code |= BTN_OK;
		if (mem_usr== 0x07) ret_code |= BTN_USR;

		// Delay, wenn gewartet wird
		if (wait)  { HAL_Delay(10); }
	}
	while ((wait!=0) && (ret_code==0));

	return (ret_code);
}


/**
 * @brief Bluetooth-Namen setzen
 *
 * @param
 * @return
 */

static void test_btname()
{
	char names[5][7]={"Speedy", "Sparky", "Spoofy", "Spacey", "Spiffy"};
	char buf[15];
	uint8_t btns;
	uint8_t idx;

	idx=0;
	display_clear();
<<<<<<< HEAD
	display_println("|  Set BT Name  |\n"
					"'---------------'");

	sprintf(buf,"Now: %s",bt_getname());
	display_println(buf);
	display_println("<UP>  Nächster\n"
					"<OK>  Setzen\n"
					"<USR> zurück");
=======
	display_println("|  Set BT Name  |");
	display_println("'---------------'");
	sprintf(buf,"Now: %s",bt_getname());
	display_println(buf);
	display_println("<UP>  Nächster");
	display_println("<OK>  Setzen");
	display_println("<USR> zurück");
>>>>>>> refs/remotes/origin/master

	while(1)
	{
		display_gotoxy(0,7);
		sprintf(buf,"NEW: %s",names[idx]);
		display_println(buf);
		btns = buttonpressed(BTN_WAIT);
		if (btns&BTN_USR)
		{
			// Funktion ohne programmieren verlassen
			return;
		}
		if (btns&BTN_UP)
		{
			idx = (idx+1)%5;
		}
		if (btns&BTN_OK)
		{
			break;	// Schleife verlassen und neuen Namen programmieren
		}
	}

	display_clear();
	display_println("Setze neuen Namen");
	display_println(names[idx]);
	bt_program(names[idx]);
}
//------------------------------------------------------------------------
// Testing Distance sensors
//------------------------------------------------------------------------

/**
 * @brief Zeichne Zumo Körper für ToF-Anzeige
 */
static void testing_draw_body()
{
	display_line_f(40,63,40,48);
	display_line_f(41,63,41,49);
	display_line_f(40,48,56,32);
	display_line_f(41,49,57,33);
	display_line_f(56,32,72,32);
	display_line_f(57,33,71,33);
	display_line_f(72,32,88,48);
	display_line_f(71,33,87,49);
	display_line_f(88,48,88,63);
	display_line_f(87,63,87,49);
}

/**
 * @brief Zeichne Ergebnis Entfernungsmessung
 *
 * @param
 *   x,y Ursprung
 *   intensity Entfernung
 *   rot Rotationswinkel des Entfernungskegels in Grad
 *
 * @return none
 */
static void testing_draw_dist(int x, int y, int intensity, int rot)
{
	int x1,x2,y1,y2;
	int i,h;
	if (intensity >320) intensity=320;

//	for (i=0;i<(intensity/20);i+=2)
	i=intensity/10;
	{
		// Rotate
		switch(rot)
		{
		case 0: 	y1=-i;
					x1=-4-i/4;
					x2=4+i/4;
					y2=y1;
					break;
		case 90:	x1=i;
					y1=-4-i/4;
					y2=4+i/4;
					x2=x1;
					break;
		case -90:	x1=-i;
					y1=-4-i/4;
					y2=4+i/4;
					x2=x1;
					break;
		case   45:  x1=(-4-i/4+i)*0.71+0.5;
					y1=(-4-i/4-i)*0.71+0.5;
					x2=(4+i/4+i)*0.71+0.5;
					y2=(4+i/4-i)*0.71+0.5;
					break;
		case  -45:  x1=(-4-i/4-i)*0.71+0.5; 	//x+y
					y1=(4+i/4-i)*0.71+0.5;	//-x+y
					x2=(4+i/4-i)*0.71+0.5;
					y2=(-4-i/4-i)*0.71+0.5;
					break;
		default:	printf("ERROR: testing_draw_dist\r\n");
					break;
		}
		if (y1>63) y1=63;
		if (y2>63) y2=63;
		display_line_f(x+x1,y+y1,x+x2,y+y2);
		display_line_f(x+x1,y+y1,x,y);
		display_line_f(x+x2,y+y2,x,y);
	}
}

/**
 *  @brief Test der ToF-Sensoren
 *
 *  @param none
 *  @return none
 */
static void test_distance()
{
	uint16_t range[5];  // FF, FL, FR, LL, RR

	display_clear();
	display_println("|  Distance Sensors  |\n"
					"'--------------------'\n"
					"<USR> Back to Main");

	HAL_Delay(1000);
	//distance_init_scan();
	distance_start_scan();
	HAL_Delay(20);

	while((buttonpressed(0)&BTN_USR) == 0)
	{
		distance_get_scan_values(range);
		distance_start_scan();
		display_clear_f();
		display_println("<USR> Back to Main");
		testing_draw_body();
		testing_draw_dist(64, 32, range[0], 0);		// FF
		testing_draw_dist(80, 40, range[2], 45);	// FR
		testing_draw_dist(47, 41, range[1], -45);	// FL
		testing_draw_dist(90, 56, range[4],90);		// RR
		testing_draw_dist(40, 56, range[3],-90);	// LL
		display_update();
		HAL_Delay(10);
	}
}

#define DEG2RAD 2.0*3.1415927/360.0
static void constrain(double *v, double min, double max)
{
	if ((*v)<min) (*v)=min;
	if ((*v)>max) (*v)=max;
}

static void test_bno055()
{
	bno055_vector_t v,v0;	// v=aktueller Wert v0=Startwert
	int x1,x2,y1,y2,x3,y3;  // Für Linie
	int x0,y0;        // Ursprung
	double w0, wp, wm;  // Hauptwinkel, Winkel plus 30 / minus 30

	display_clear();
	display_println("|  Test IMU-Sensor  |\n"
					"'-------------------'\n"
					"<USR> Back to Main");
	HAL_Delay(1000);

	// Orientierung zu Beginn als Bezug ...
	v0= bno055_getVectorEuler();

	while((buttonpressed(0)&BTN_USR) == 0)
	{
	    v= bno055_getVectorEuler();
	    constrain(&v.y,-48.,48.);
	    constrain(&v.z,-22.,22.);
	    x0 = 64. - (v.y-v0.y);
	    y0 = 32. - (v.z-v0.z)/2.;
        w0 = v.x-v0.x;
        wp = (w0+30.)*DEG2RAD;
        wm = (w0-30.)*DEG2RAD;
        w0 *= DEG2RAD;
	    display_clear_f();
	    x1 = x0-10*sin(w0);
	    y1 = y0-10*cos(w0);
	    x2 = x0+10*sin(w0);
	    y2 = y0+10*cos(w0);
	    display_line_f(x1,y1,x2,y2);
	    x2 = x0-5*sin(wp);
	    y2 = y0-5*cos(wp);
	    display_line_f(x1,y1,x2,y2);
	    x3 = x0-5*sin(wm);
	    y3 = y0-5*cos(wm);
	    display_line_f(x1,y1,x3,y3);
	    display_line_f(x2,y2,x3,y3);
	    //sprintf(buf,"%4i/%4i/%4i",(int)v.x, (int)v.y, (int)v.z);
	    //display_println(buf);
	    display_println("<USR> Back to main");
	    HAL_Delay(20);
	}
}
