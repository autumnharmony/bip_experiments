/*
	(С) Волков Максим 2019 ( Maxim.N.Volkov@ya.ru )

	Календарь v1.0
	Приложение простого календаря.
	Алгоритм вычисления дня недели работает для любой даты григорианского календаря позднее 1583 года.
	Григорианский календарь начал действовать в 1582 — после 4 октября сразу настало 15 октября.

	Календарь от 1600 до 3000 года
	Функции перелистывания каленаря вверх-вниз - месяц, стрелками год
	При нажатии на название месяца устанавливается текущая дата


	v.1.1
	- исправлены переходы в при запуске из бстрого меню

*/

#include <libbip.h>
#include "buttons.h"
#define DEBUG_LOG

//	структура меню экрана календаря
struct regmenu_ menu_calend_screen = {
						55,
						1,
						0,
						dispatch_calend_screen,
						key_press_calend_screen,
						calend_screen_job,
						0,
						show_calend_screen,
						0,
						0
					};

int main(int param0, char** argv){	//	переменная argv не определена
	show_calend_screen((void*) param0);
}

void show_calend_screen (void *param0){
struct calend_** 	calend_p = get_ptr_temp_buf_2(); 	//	указатель на указатель на данные экрана
struct calend_ *	calend;								//	указатель на данные экрана
struct calend_opt_ 	calend_opt;							//	опции календаря

#ifdef DEBUG_LOG
log_printf(5, "[show_calend_screen] param0=%X; *temp_buf_2=%X; menu_overlay=%d", (int)param0, (int*)get_ptr_temp_buf_2(), get_var_menu_overlay());
log_printf(5, " #calend_p=%X; *calend_p=%X", (int)calend_p, (int)*calend_p);
#endif

if ( (param0 == *calend_p) && get_var_menu_overlay()){ // возврат из оверлейного экрана (входящий звонок, уведомление, будильник, цель и т.д.)

#ifdef DEBUG_LOG
	log_printf(5, "  #from overlay");
	log_printf(5, "\r\n");
#endif

	calend = *calend_p;						//	указатель на данные необходимо сохранить для исключения
											//	высвобождения памяти функцией reg_menu
	*calend_p = NULL;						//	обнуляем указатель для передачи в функцию reg_menu

	// 	создаем новый экран, при этом указатели temp_buf_1 и temp_buf_2 были равны 0 и память не была высвобождена
	reg_menu(&menu_calend_screen, 0); 		// 	menu_overlay=0

	*calend_p = calend;						//	восстанавливаем указатель на данные после функции reg_menu

	draw_buttons();

} else { 			// если запуск функции произошел из меню,

#ifdef DEBUG_LOG
	log_printf(5, "  #from menu");
	log_printf(5, "\r\n");
#endif
	// создаем экран
	reg_menu(&menu_calend_screen, 0);

	// выделяем необходимую память и размещаем в ней данные
	*calend_p = (struct calend_ *)pvPortMalloc(sizeof(struct calend_));
	calend = *calend_p;		//	указатель на данные

	// очистим память под данные
	_memclr(calend, sizeof(struct calend_));

	calend->proc = param0;

	// запомним адрес указателя на функцию в которую необходимо вернуться после завершения данного экрана
	if ( param0 && calend->proc->elf_finish ) 			//	если указатель на возврат передан, то возвоащаемся на него
		calend->ret_f = calend->proc->elf_finish;
	else					//	если нет, то на циферблат
		calend->ret_f = show_watchface;

	// struct datetime_ datetime;
	// _memclr(&datetime, sizeof(struct datetime_));
  //
	// 	// получим текущую дату
	// get_current_date_time(&datetime);

	// calend->day 	= datetime.day;
	// calend->month 	= datetime.month;
	// calend->year 	= datetime.year;

	// считаем опции из flash памяти, если значение в флэш-памяти некорректное то берем первую схему
	// текущая цветовая схема хранится о смещению 0
	// ElfReadSettings(calend->proc->index_listed, &calend_opt, OPT_OFFSET_CALEND_OPT, sizeof(struct calend_opt_));

	// if (calend_opt.color_scheme < COLOR_SCHEME_COUNT)
	// 		calend->color_scheme = calend_opt.color_scheme;
	// else
	// 		calend->color_scheme = 0;

	draw_buttons();
}

// при бездействии погасить подсветку и не выходить
set_display_state_value(8, 1);
set_display_state_value(2, 1);

// таймер на job на 20с где выход.
set_update_period(1, INACTIVITY_PERIOD);

}

void draw_buttons(){
struct calend_** 	calend_p = get_ptr_temp_buf_2(); 		//	указатель на указатель на данные экрана
struct calend_ *	calend = *calend_p;						//	указатель на данные экрана


											 // {COLOR_SH_BLACK, COLOR_SH_YELLOW, COLOR_SH_AQUA, COLOR_SH_WHITE, COLOR_SH_RED, COLOR_SH_WHITE, COLOR_SH_WHITE,
											 // COLOR_SH_GREEN, COLOR_SH_BLACK, COLOR_SH_AQUA, COLOR_SH_YELLOW, COLOR_SH_BLACK, COLOR_SH_WHITE, COLOR_SH_YELLOW, COLOR_SH_BLACK},
                        // {COLOR_SH_WHITE, COLOR_SH_BLACK, COLOR_SH_BLUE, COLOR_SH_BLACK, COLOR_SH_RED, COLOR_SH_WHITE, COLOR_SH_BLACK,
											 // COLOR_SH_BLUE, COLOR_SH_WHITE, COLOR_SH_AQUA, COLOR_SH_BLACK, COLOR_SH_WHITE, COLOR_SH_RED, COLOR_SH_BLUE, COLOR_SH_WHITE},
											 // COLOR_SH_GREEN, COLOR_SH_RED, COLOR_SH_AQUA, COLOR_SH_YELLOW, COLOR_SH_RED, COLOR_SH_WHITE, COLOR_SH_AQUA, COLOR_SH_BLACK},
                       // {COLOR_SH_BLACK, COLOR_SH_YELLOW, COLOR_SH_AQUA, COLOR_SH_WHITE, COLOR_SH_RED, COLOR_SH_WHITE, COLOR_SH_WHITE,
		                     // {COLOR_SH_WHITE, COLOR_SH_BLACK, COLOR_SH_BLUE, COLOR_SH_BLACK, COLOR_SH_RED, COLOR_SH_WHITE, COLOR_SH_BLACK,
                         // COLOR_SH_BLUE, COLOR_SH_RED, COLOR_SH_BLUE, COLOR_SH_BLACK, COLOR_SH_RED, COLOR_SH_BLACK, COLOR_SH_BLUE, COLOR_SH_WHITE},
	                       // {COLOR_SH_BLACK, COLOR_SH_YELLOW, COLOR_SH_AQUA, COLOR_SH_WHITE, COLOR_SH_RED, COLOR_SH_WHITE, COLOR_SH_WHITE,
	                        // COLOR_SH_GREEN, COLOR_SH_BLACK, COLOR_SH_AQUA, COLOR_SH_YELLOW, COLOR_SH_BLACK, COLOR_SH_WHITE, COLOR_SH_AQUA|(1<<7), COLOR_SH_BLACK},

// int color_scheme[COLOR_SCHEME_COUNT][15];

char text_buffer[24];

_memclr(&text_buffer, 24);

set_bg_color(color_scheme[calend->color_scheme][CALEND_COLOR_BG]);	//	фон календаря
fill_screen_bg();

set_graph_callback_to_ram_1();
load_font(); // подгружаем шрифты

int calend_name_height = get_text_height();

set_fg_color(color_scheme[calend->color_scheme][CALEND_COLOR_SEPAR]);


  set_fg_color(COLOR_SH_GREEN);
	draw_filled_rect_bg(0, 0, 88, 88);

  set_fg_color(COLOR_SH_YELLOW);
  draw_filled_rect_bg(88, 0, 176, 88);

  set_fg_color(COLOR_SH_RED);
  draw_filled_rect_bg(0, 88, 88, 176);

  set_fg_color(COLOR_SH_AQUA);
  draw_filled_rect_bg(88, 88, 176, 176);


	// вывод названий дней недели. если ширина названия больше чем ширина поля, выводим короткие названия
	// if (text_width(weekday_string[1]) <= (WIDTH - 2))
		text_out("1", 44,  44 );
    text_out("2", 88,  44 );
    text_out("3", 44,  88 );
    text_out("4", 88,  88 );

};


void key_press_calend_screen(){
	struct calend_** 	calend_p = get_ptr_temp_buf_2(); 		//	указатель на указатель на данные экрана
	struct calend_ *	calend = *calend_p;			//	указатель на данные экрана

	show_menu_animate(calend->ret_f, (unsigned int)show_calend_screen, ANIMATE_RIGHT);
};


void calend_screen_job(){
	struct calend_** 	calend_p = get_ptr_temp_buf_2(); 		//	указатель на указатель на данные экрана
	struct calend_ *	calend = *calend_p;			//	указатель на данные экрана

	// при достижении таймера обновления выходим
	show_menu_animate(calend->ret_f, (unsigned int)show_calend_screen, ANIMATE_LEFT);
}

int dispatch_calend_screen (void *param){
	struct calend_** 	calend_p = get_ptr_temp_buf_2(); 		//	указатель на указатель на данные экрана
	struct calend_ *	calend = *calend_p;			//	указатель на данные экрана

	struct calend_opt_ calend_opt;					//	опции календаря

	struct datetime_ datetime;
	// получим текущую дату


	get_current_date_time(&datetime);
	unsigned int day;

	//	char text_buffer[32];
	 struct gesture_ *gest = param;
	 int result = 0;

	switch (gest->gesture){
		case GESTURE_CLICK: {

			// вибрация при любом нажатии на экран
			vibrate (1, 40, 0);


			if ( gest->touch_pos_y < CALEND_Y_BASE ){ // кликнули по верхней строке
				if (gest->touch_pos_x < 44){
					if ( calend->year > 1600 ) calend->year--;
				} else
				if (gest->touch_pos_x > (176-44)){
					if ( calend->year < 3000 ) calend->year++;
				} else {
					calend->day 	= datetime.day;
					calend->month 	= datetime.month;
					calend->year 	= datetime.year;
				}

				 if ( (calend->year == datetime.year) && (calend->month == datetime.month) ){
					day = datetime.day;
				 } else {
					day = 0;
				 }
					draw_month(day, calend->month, calend->year);
					repaint_screen_lines(1, 176);

			} else { // кликнули в календарь

				calend->color_scheme = ((calend->color_scheme+1)%COLOR_SCHEME_COUNT);

				// сначала обновим экран
				if ( (calend->year == datetime.year) && (calend->month == datetime.month) ){
					day = datetime.day;
				 } else {
					day = 0;
				 }
					draw_month(day, calend->month, calend->year);
					repaint_screen_lines(1, 176);

				// потом запись опций во flash память, т.к. это долгая операция
				// TODO: 1. если опций будет больше чем цветовая схема - переделать сохранение, чтобы сохранять перед выходом.
				calend_opt.color_scheme = calend->color_scheme;

				// запишем настройки в флэш память
				ElfWriteSettings(calend->proc->index_listed, &calend_opt, OPT_OFFSET_CALEND_OPT, sizeof(struct calend_opt_));
			}

			// продлить таймер выхода при бездействии через INACTIVITY_PERIOD с
			set_update_period(1, INACTIVITY_PERIOD);
			break;
		};

		case GESTURE_SWIPE_RIGHT: 	//	свайп направо
		case GESTURE_SWIPE_LEFT: {	// справа налево

			if ( get_left_side_menu_active()){
					set_update_period(0,0);

					void* show_f = get_ptr_show_menu_func();

					// запускаем dispatch_left_side_menu с параметром param в результате произойдет запуск соответствующего бокового экрана
					// при этом произойдет выгрузка данных текущего приложения и его деактивация.
					dispatch_left_side_menu(param);

					if ( get_ptr_show_menu_func() == show_f ){
						// если dispatch_left_side_menu отработал безуспешно (листать некуда) то в show_menu_func по прежнему будет
						// содержаться наша функция show_calend_screen, тогда просто игнорируем этот жест

						// продлить таймер выхода при бездействии через INACTIVITY_PERIOD с
						set_update_period(1, INACTIVITY_PERIOD);
						return 0;
					}


					//	если dispatch_left_side_menu отработал, то завершаем наше приложение, т.к. данные экрана уже выгрузились
					// на этом этапе уже выполняется новый экран (тот куда свайпнули)


					Elf_proc_* proc = get_proc_by_addr(main);
					proc->ret_f = NULL;

					elf_finish(main);	//	выгрузить Elf из памяти
					return 0;
				} else { 			//	если запуск не из быстрого меню, обрабатываем свайпы по отдельности
					switch (gest->gesture){
						case GESTURE_SWIPE_RIGHT: {	//	свайп направо
							return show_menu_animate(calend->ret_f, (unsigned int)show_calend_screen, ANIMATE_RIGHT);
							break;
						}
						case GESTURE_SWIPE_LEFT: {	// справа налево
							//	действие при запуске из меню и дальнейший свайп влево


							break;
						}
					} /// switch (gest->gesture)
				}

			break;
		};	//	case GESTURE_SWIPE_LEFT:


		case GESTURE_SWIPE_UP: {	// свайп вверх
			if ( calend->month < 12 )
					calend->month++;
			else {
					calend->month = 1;
					calend->year++;
			}

			if ( (calend->year == datetime.year) && (calend->month == datetime.month) )
				day = datetime.day;
			else
				day = 0;
			draw_month(day, calend->month, calend->year);
			repaint_screen_lines(1, 176);

			// продлить таймер выхода при бездействии через INACTIVITY_PERIOD с
			set_update_period(1, INACTIVITY_PERIOD);
			break;
		};
		case GESTURE_SWIPE_DOWN: {	// свайп вниз
			if ( calend->month > 1 )
					calend->month--;
			else {
					calend->month = 12;
					calend->year--;
			}

			if ( (calend->year == datetime.year) && (calend->month == datetime.month) )
				day = datetime.day;
			else
				day = 0;
			draw_month(day, calend->month, calend->year);
			repaint_screen_lines(1, 176);

			// продлить таймер выхода при бездействии через INACTIVITY_PERIOD с
			set_update_period(1, INACTIVITY_PERIOD);
			break;
		};
		default:{	// что-то пошло не так...
			break;
		};

	}


	return result;
};
