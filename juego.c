#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "animos.h"
#include "defendiendo_torres.h"
#include "utiles.h"

//  CONSTANTES DE JUEGO (!)
    static const int ESTADO_JUGANDO = 0;
    static const int ESTADO_GANADO  = 1;
    //static const int ESTADO_PERDIDO =-1;
    static const int CANTIDAD_NIVELES = 4;

    static const int RES_ORCO  = 200;
    static const int RES_ORCO_RAND  = 100; 

    typedef struct config_nivel{
        int num;
        int dimension;
        char entrada;
        bool torre_1, torre_2;
        int enanos, elfos, orcos;
        int turnos_bonus;

    } config_nivel_t;

    #define ENTRADA_ESTE  'E'
    #define ENTRADA_OESTE 'O'
    #define ENTRADA_NORTE 'N'
    #define ENTRADA_SUR   'S'

    const config_nivel_t NIVEL_1 ={ 
        .num = 1,
        .dimension = 15,
        .entrada = ENTRADA_ESTE,
        .torre_1 = true, .torre_2 = false,
        .enanos = 5, .elfos = 0, 
        .orcos = 100,
        .turnos_bonus = 25
    };

    const config_nivel_t NIVEL_2 ={ 
        .num = 2,
        .dimension = 15,
        .entrada = ENTRADA_OESTE,
        .torre_1 = false, .torre_2 = true,
        .enanos = 0, .elfos = 5, 
        .orcos = 200,
        .turnos_bonus = 50
    };

    const config_nivel_t NIVEL_3 ={ 
        .num = 3,
        .dimension = 20,
        .entrada = ENTRADA_NORTE,
        .torre_1 = true, .torre_2 = true,
        .enanos = 3, .elfos = 3, 
        .orcos = 300,
        .turnos_bonus = 50
    };

    const config_nivel_t NIVEL_4 ={ 
        .num = 4,
        .dimension = 20,
        .entrada = ENTRADA_SUR,
        .torre_1 = true, .torre_2 = true,
        .enanos = 4, .elfos = 4, 
        .orcos = 250,
        .turnos_bonus = 25
    };

    const int MAX_INTENTOS = 20;
    const int COMPLEJIDAD_NIVELES = 2;
    const int RAREZA_CRUZADO = 2;
//  CONSTANTES DE JUEGO (¡)

// HEADER DE MENU Y CONFIG (!)
    
  // MENU

    #define OPCION_NUEVO_JUEGO          1
    #define OPCION_INICIALIZAR_ANIMOS   2
    #define OPCION_MOSTRAR_OPCIONES     3
    #define OPCION_SALIR                4
    #define OPCION_TESTEO               9

    /*
     * Muestra un menu y actualiza la opcion elegida :
     * 1 > Comenzar un nuevo juego
     * 2 > Inicializar los animos / humedad / viento
     * 3 > Opciones ( DLC )
     * 4 > Salir
     */
    void menu( int* opcion );

  // CONFIG

    typedef struct config {
        float velocidad;
        int bonus_resistencia;
    } config_t;

    const config_t CONFIG_STD ={
        .velocidad = 0.4f,
        .bonus_resistencia = 0
    };

    // Inicializa la config son los valores std
    void iniciar_config( config_t* config );   
// HEADER DE MENU Y CONFIG (!)


// HEADER JUEGO (!)
    /* 
     * Comienza un nuevo juego
     * pre: recibe un juego inicializado
     */
    void nuevo_juego( juego_t* juego , config_t config );

    // muestra opciones/configuracion
    void mostrar_opciones( juego_t* juego, config_t* config );

    /*
     * Devuelve un nivel_t segun el nivel(numero)
     *  0 > Entrada Este
     *  1 > Entrada Oeste
     *  2 > Entrada Norte
     *  3 > Entrada Sur
     *  Default > Nivel vacio
     */
    nivel_t nuevo_nivel( int nivel );

    // Muestra un mensaje al pasar de nivel
    void mensaje_nuevo_nivel( int nivel );
// HEADER JUEGO (¡)


int main(){
    srand( (unsigned int) time(NULL));  // ERROR: unsigned -> integer
    //srand(105774); 

    config_t config;
    iniciar_config( &config );

    int viento = 0;
    int humedad = 0;
    char animo_legolas = 'B';
    char animo_gimli = 'B';

    juego_t juego;

    int opcion = OPCION_INICIALIZAR_ANIMOS;

	while( opcion != OPCION_SALIR ){
        menu( &opcion );

        switch( opcion ){

            case OPCION_TESTEO:
                opcion = OPCION_NUEVO_JUEGO;
                config.velocidad = 0.1f;
                config.bonus_resistencia = 1000;

            case OPCION_NUEVO_JUEGO:
                inicializar_juego(&juego, viento, humedad, animo_legolas, animo_gimli);
                nuevo_juego( &juego , config );
            break;

            case OPCION_INICIALIZAR_ANIMOS:
                animos(&viento , &humedad , &animo_legolas , &animo_gimli);
            break;

            case OPCION_MOSTRAR_OPCIONES:
                mostrar_opciones( &juego, &config );
            break;

        }

    }

	return 0;
}

// MENU Y CONFIG (!)

    void menu( int* opcion ){

        system("clear");
        printf("\n");
        printf("----------------------------------------------------\n");
        printf("-------------  DEFENDIENDO LAS TORRES  -------------\n");
        printf("----------------------------------------------------\n");
        printf("\n");
        printf("%i: Nuevo Juego \n", OPCION_NUEVO_JUEGO);
        printf("%i: Animos \n", OPCION_INICIALIZAR_ANIMOS);
        printf("%i: Opciones \n", OPCION_MOSTRAR_OPCIONES);
        printf("%i: Salir \n", OPCION_SALIR);

        char input[20];
        scanf("%s",input);
        *opcion = input[0]-48;
    }

    void mostrar_opciones( juego_t* juego , config_t* config ){

        int opcion = 0;

        system("clear");
        printf("\n");
        printf("----------------------------------------------------\n");
        printf("-------------         OPCIONES         -------------\n");
        printf("----------------------------------------------------\n");
        printf("\n");
        printf("1: Velocidad - [FRECUENCIA : %f] \n",config->velocidad);
        printf("2: Regeneracion por nivel - [BONUS : %i]\n",config->bonus_resistencia);
        printf("3: Revivir a Sauron \n");
        printf("4: Salir \n");

        char input[20];
        scanf("%s",input);
        opcion = input[0]-48;

        switch( opcion ){
            case 1:
                printf("Ingrese la FRECUENCIA [ 0.1 - 1.0 ]  > ");
                scanf("%f", &(config->velocidad) );
            break;

            case 2:
                printf("Ingrese el BONUS [ 0 - 1000 ]  > ");
                scanf("%i", &(config->bonus_resistencia) );
            break;
        }
        return;
    }

    void iniciar_config( config_t* config ){
        *config = CONFIG_STD;
    }
// MENU Y CONFIG (!)

// JUEGO (!)
    void nuevo_juego( juego_t* juego , config_t config ){

        juego->nivel_actual = 0;
        juego->nivel.tope_enemigos = 0;

        while( estado_juego( *juego) == ESTADO_JUGANDO ){

            if( estado_nivel( juego->nivel ) == ESTADO_GANADO  ){
                
                juego->nivel_actual ++;
                juego->nivel = nuevo_nivel( juego->nivel_actual );
                mensaje_nuevo_nivel( juego->nivel_actual );

                juego->torres.resistencia_torre_1 += config.bonus_resistencia;
                juego->torres.resistencia_torre_2 += config.bonus_resistencia;

            }else{

                jugar_turno( juego );

                mostrar_juego( *juego );
                
                detener_el_tiempo( config.velocidad );

            }

        }

        // GANO O PERDIO 
    }

    void mensaje_nuevo_nivel( int nivel ){

        system("tput clear");

        if( nivel > 0)
            printf("\n\t HAS COMPLETADO EL NIVEL %i \n", nivel-1 );

        if( nivel <= CANTIDAD_NIVELES )
            printf("\n\t COMIENZA EL NIVEL %i \n", nivel );

        printf("\n\t presione para continuar \n" );

        char c;
        scanf("%c",&c);
    }

    nivel_t nuevo_nivel( int nivel ){

        int dimension; // mapa de dim x dim
        nivel_t nuevo_nivel;

        nuevo_nivel.tope_camino_1 = 0;
        nuevo_nivel.tope_camino_2 = 0;
        nuevo_nivel.tope_defensores = 0;
        nuevo_nivel.tope_enemigos = 0;

        config_nivel_t config;
        config.num = 0;

        switch( nivel ){

            case 1:
                config = NIVEL_1;
            break;

            case 2:
                config = NIVEL_2;
            break;

            case 3:
                config = NIVEL_3;
            break;

            case 4:
                config = NIVEL_4;
            break;
        }

        
        if( nivel <= CANTIDAD_NIVELES ){

            int intentos;

            bool cruzado = ( rand()%RAREZA_CRUZADO == 0 );

            dimension = config.dimension ;

            nuevo_nivel.tope_enemigos = config.orcos;

            coordenada_t entrada,torre;

            //CAMINO 1
            if( config.torre_1 ){

                switch( config.entrada ){

                    case ENTRADA_ESTE:
                        entrada.fil = rand()%dimension;
                        entrada.col = dimension-1;
                        torre.fil = rand()%dimension;
                        torre.col = 0;
                    break;

                    case ENTRADA_NORTE:
                        entrada.fil = 0;
                        entrada.col = rand()%(dimension/2)+(dimension/2)*(!cruzado);
                        torre.fil = dimension-1;
                        torre.col = rand()%(dimension/2);
                    break;

                    case ENTRADA_SUR:
                        entrada.fil = dimension-1;
                        entrada.col = rand()%(dimension/2)+(dimension/2)*(!cruzado);
                        torre.fil = 0;
                        torre.col = rand()%(dimension/2);
                    break;

                }

                intentos = 0;
                nuevo_nivel.tope_camino_1 = 0;

                while( (nuevo_nivel.tope_camino_1 < COMPLEJIDAD_NIVELES *dimension) 
                    && (intentos < MAX_INTENTOS) ){

                    obtener_camino(
                        nuevo_nivel.camino_1,
                        &nuevo_nivel.tope_camino_1, 
                        entrada, torre
                    );
                }

            }else
                nuevo_nivel.tope_camino_1 = 0;

            //CAMINO 2
            if( config.torre_2 ){

                switch( config.entrada ){

                    case ENTRADA_OESTE:
                        entrada.fil = rand()%dimension;
                        entrada.col = 0;
                        torre.fil = rand()%dimension;
                        torre.col = dimension-1;
                    break;

                    case ENTRADA_NORTE:
                        entrada.fil = 0;
                        entrada.col = rand()%(dimension/2)+(dimension/2)*cruzado;
                        torre.fil = dimension-1;
                        torre.col = rand()%(dimension/2)+(dimension/2);

                    break;

                    case ENTRADA_SUR:
                        entrada.fil = dimension-1;
                        entrada.col = rand()%(dimension/2)+(dimension/2)*cruzado;
                        torre.fil = 0;
                        torre.col = rand()%(dimension/2)+(dimension/2);
                    break;

                }

                intentos = 0;
                nuevo_nivel.tope_camino_2 = 0;

                while( (nuevo_nivel.tope_camino_2 < COMPLEJIDAD_NIVELES*dimension) 
                    && (intentos < MAX_INTENTOS) ){

                    obtener_camino(
                        nuevo_nivel.camino_2,
                        &nuevo_nivel.tope_camino_2, 
                        entrada, torre
                    );
                }

            }else
                nuevo_nivel.tope_camino_2 = 0;      
        }


        for(int i = 0; i<nuevo_nivel.tope_enemigos; i++){
            nuevo_nivel.enemigos[i].vida= RES_ORCO + rand() %(RES_ORCO_RAND+1) ;
            nuevo_nivel.enemigos[i].camino = 0;
            nuevo_nivel.enemigos[i].pos_en_camino = -1;
        }

        return nuevo_nivel;
    }
// JUEGO (¡)

