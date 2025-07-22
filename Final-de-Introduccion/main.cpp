#include <SDL.h>
#include<SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include<SDL_mixer.h>
#include<SDL_image.h>


using namespace std;

// Ints constantes para ancho y alto de la ventana
const int WIN_WIDTH = 1200;
const int WIN_HEIGHT = 980;





// funcion para cargar personajes desde el archivo
vector<string> loadCharacter(const string& filename)
{
    vector<string> characters;
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "error al abrir el archivo : no se pudo abrir " << filename << endl;
    }

    string line;
    while (getline(file, line))
    {
        if (!line.empty()) {

            characters.push_back(line);

        }
    }
    file.close();
    return characters;

}









// la funcion "main" 
// tiene que recibir si o si estos argumentos: int argc, char* argv[],
int main(int argc, char* argv[]) {


    int selectedCharacter = 0;



    // Inicializa SDL, y todo lo que tiene que ver con el
    // y al mismo tiempo chequea por errores
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << "SDL_Init Error: " << SDL_GetError() << endl;
        return 1;
    }

    // inicializa SDL.ttf
    if (TTF_Init() < 0)
    {
        cerr << "SDL_Init error: " << TTF_GetError << endl;
        return 1;
    }


    // inicializa mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cerr << "MIX_OpenAudio Error: " << Mix_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // cargar la musica
    Mix_Music* mainMusic = Mix_LoadMUS("assets/mainTheme.mp3");
    if (!mainMusic)
    {
        cerr << "Mix_LoadMus Error: " << Mix_GetError() << endl;
        Mix_Quit();
        SDL_Quit();
        return 1;
    }
    Mix_VolumeMusic(32); // volumen de la musica de fondo

    // reproduce el MP3 en bucle (-1 para bucle infinito y 0 para una unica vez)
    if (Mix_PlayMusic(mainMusic, -1) < 0) 
    {
        cerr << "Mix_PlayMusic Error: " << Mix_GetError() << endl;
        Mix_FreeMusic(mainMusic);
        Mix_Quit();
        SDL_Quit();
        return 1;
    }

    // Carga el sonido de movimiento del marcador ----------------------
    Mix_Chunk* moveSound = Mix_LoadWAV("assets/selectorSong2.mp3");
    if (!moveSound)
    {
        cerr << "Mix_LoadWAV Error: " << Mix_GetError() << endl;
        Mix_FreeMusic(mainMusic);
        Mix_Quit();
        SDL_Quit();
        return 1;
    }

    //------------------ Carga el sonido de borrado de personaje ---------
    Mix_Chunk* byeSound = Mix_LoadWAV("assets/ByeSong.mp3");
    if (!byeSound)
    {
        cerr << "Mix_LoadWAV Error: " << Mix_GetError() << endl;
        Mix_FreeMusic(mainMusic);
        Mix_Quit();
        SDL_Quit();
        return 1;
    }
    //----------------------------------------------------------------------
    
    // Crea la ventana con algunas propiedades
    SDL_Window* window = SDL_CreateWindow(
        "SDL2",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIN_WIDTH, WIN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    // Crea el renderizador que va a ir con la ventana que creamos con algunas propiedades
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);


    // textura de triangulo ----------------------------------------

    SDL_Rect marcador;
    marcador.x = 450;
    marcador.y = 50;
    const char* imagePath = "assets/Puntero.png";
    SDL_Surface* surface = IMG_Load(imagePath);
    if (!surface)
    {
        SDL_Log("error: %s", IMG_GetError());
    }
    marcador.h = surface->h;
    marcador.w = surface->w;
    SDL_Texture* textureMarc = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!textureMarc)
    {
        cout << "no se cargo la imagen";
    }

    // Carga la fuente para el texto desde la fuente de windows
    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 28);
    if (!font) // Verifica si la fuente se cargó correctamente
    {
        cerr << "TTF_OpenFont Error: " << TTF_GetError() << endl;
        cerr << "Directorio actual: " << SDL_GetBasePath() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // carga los personajes desde el archivo-------------------------

    vector<string> characters = loadCharacter("characters.txt");
    if (characters.empty())
    {
        cerr << "NO SE CARAGRON LOS PERSONAJES DESDE Characters.txt";
    }
    else if (characters.size() != 6) 
    {

        cerr << "SE SUPONIA QUE SERIAN 6 PERSONAJES, PERO SE CARGARON";

    }

    //---------------------------------------------------------------


    // linea superior-------------------------------

    SDL_Rect superior;

    superior.x = 0;
    superior.y = 150;
    superior.w = 1300;
    superior.h = 1;

    //linea inferior----------------------------------

    SDL_Rect inferior;

    inferior.x = 0;
    inferior.y = 870;
    inferior.w = 1300;
    inferior.h = 1;

    //------------------------------------------------

    // Primer While -> Loop Principal
    // Segundo While -> Loop para Eventos
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }

            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                quit = true;
            }


            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_UP && selectedCharacter > 0)
                {
                    selectedCharacter--;
                    Mix_PlayChannel(-1, moveSound, 0);
                }
                else if (event.key.keysym.sym == SDLK_DOWN && selectedCharacter < characters.size() - 1)
                {
                    selectedCharacter++;
                    Mix_PlayChannel(-1, moveSound, 0); // reproduce el sonido
                }
                else if (event.key.keysym.sym == SDLK_SPACE && !characters.empty())
                {
                    characters.erase(characters.begin() + selectedCharacter); // si el espacio de la lista no esta vacio borra lo marcado 
                    if (selectedCharacter >= characters.size() && !characters.empty()) // al eliminar y si la lista no está vacía, decrementa selectedCharacter para apuntar al personaje anterior.


                    {
                        selectedCharacter--;
                    }
                    if (characters.empty()) // Si la lista queda vacía, establece selectedCharacter = 0 para reiniciar la selección.


                    {
                        selectedCharacter = 0;
                    }
                    Mix_PlayChannel(-1, byeSound, 0);
                }
            }

        }

       
        // Pinta la ventana de un color (Negro)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        // Limpia la ventana
        SDL_RenderClear(renderer);
        

        //---------------------------------- TITULO DE SELECCION DE PERSONAJE ------------------------------------------------------

        SDL_Color textColor = { 255, 255, 255, 255 };
        const char* mainTitleText = "LIST OF FIGHTERS";
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, mainTitleText, textColor);

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (!textTexture) {
            cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << endl;
            SDL_FreeSurface(textSurface);
            continue;
        }

        int textW = textSurface->w;
        int textH = textSurface->h;
        SDL_Rect textRect = { (WIN_WIDTH - textW) / 2, 120, textW, textH };
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        // Leyenda de uso de movimiento-----------------------------------------------------------------------------------------------

        
        const char* manualText = "press UP/DOWN to select fighter                  press SpaceBar to remove fighter on the list";
        SDL_Surface* textSurface2 = TTF_RenderText_Solid(font, manualText, textColor);
        SDL_Texture* textTexture2 = SDL_CreateTextureFromSurface(renderer, textSurface2);
        if (!textTexture2) {
            cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << endl;
            SDL_FreeSurface(textSurface2);
            continue;
        }
        int textw = textSurface2->w;
        int texth = textSurface2->h;
        SDL_Rect textRect2 = { (WIN_WIDTH - textw) / 2, 820, textw, texth };
        SDL_RenderCopy(renderer, textTexture2, NULL, &textRect2);

        //--------------------------- RENDERIZA LOS NOMBRES DE PERSONAJES ------------------------------------------------------------

        int yPosition = 260; // establece la posicion vertical inicial
        int characterIndex = 0; // lleva la cuenta del indice del personaje mientras se itera// para saber cual personaje esta seleccionado
        for ( const auto& character : characters) // Itera sobre el vector characters, que contiene los nombres de los personajes cargados desde el archivo "characters.txt". 
        { // const auto& asegura que no se copie cada string innecesariamente y evita modificaciones accidentales.

            textSurface = TTF_RenderText_Solid(font, character.c_str(), textColor);
            if (!textTexture)
            {
                cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << endl;
                continue;
            }
            textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (!textTexture)
            {
                cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << endl;
                SDL_FreeSurface(textSurface);
                continue;
            }
            textW = textSurface->w;
            textH = textSurface->h;
            SDL_Rect charRect = { (WIN_WIDTH - textW) / 2, yPosition, textW, textH };
            SDL_RenderCopy(renderer, textTexture, NULL, &charRect);
            if (characterIndex == selectedCharacter)
            {
                marcador.y = yPosition;
            }
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);

            yPosition += textH + 20; // Espacio entre nombres
            characterIndex++;
        }

        if (!characters.empty())
        {
            SDL_RenderCopy(renderer, textureMarc, NULL, &marcador);
        }




        //------------------------------------------------ RENDERIZA LAS LINEAS DE LOS EXTREMOS Y -------------------------------------

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &superior);
        SDL_RenderDrawRect(renderer, &inferior);

        // RENDERIZA EL MARCADOR ------------------------------------------

        SDL_RenderCopy(renderer, textureMarc,NULL, &marcador);

        //-----------------------------------------------------------------------------------------------------------------------------




        // El renderizador muestra lo que tenga pendiente
        SDL_RenderPresent(renderer);
    }

    // Limpia la ventana y la memoria
    TTF_CloseFont(font); // Liberar la fuente
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}