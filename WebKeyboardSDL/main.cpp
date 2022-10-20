#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>

#define KEY_DOWN   0b00000001
#define KEY_UP     0b00000010
#define MOUSE_MOVE 0b00000100
#define MOUSE_DOWN 0b00001000
#define MOUSE_UP   0b00010000
#define FOCUS      0b00100000

struct t_event {
    uint8_t action;
    uint8_t code;
    uint16_t x, y;
    uint16_t crc;
};


SDL_Window* init() {
    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        fprintf( stderr, "could not initialize sdl2: %s\n", SDL_GetError() );
        exit( 1 );
    }
    SDL_Window* window = SDL_CreateWindow( "KeyBoard ",
        SDL_WINDOWPOS_UNDEFINED_DISPLAY( 1 ), SDL_WINDOWPOS_UNDEFINED,
        5120, 1440,
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS
        | SDL_WINDOW_FULLSCREEN_DESKTOP
        // | SDL_WINDOW_SKIP_TASKBAR
        | SDL_WINDOW_KEYBOARD_GRABBED
        // | SDL_WINDOW_UTILITY
        // | SDL_WINDOW_FULLSCREEN
    );
    if (window == NULL) {
        fprintf( stderr, "could not create window: %s\n", SDL_GetError() );
        exit( 1 );
    }
    return window;
}

IPaddress initNetwork() {
    if (SDLNet_Init() == -1) {
        fprintf( stderr, "ER: SDLNet_Init: %sn", SDLNet_GetError() );
        exit( -1 );
    }
    IPaddress ip;
    if (SDLNet_ResolveHost( &ip, "192.168.10.1", 2300 ) != 0) {
        fprintf( stderr, "Erreur de resolution d'IP\n" );
        exit( 1 );
    }
    return ip;
}

bool quit = false;

void send( IPaddress ip, t_event event ) {
    TCPsocket serveur = SDLNet_TCP_Open( &ip );
    if (serveur == NULL) {
        fprintf( stderr, "Erreur de connexion au serveur : %s\n", SDLNet_GetError() );
    } else {
        fprintf( stdout, "Send : %d %d %d %d %d\n", event.action, event.code, event.x, event.y, event.crc );
        if (SDLNet_TCP_Send( serveur, &event, sizeof( event ) ) < sizeof( event )) {
            fprintf( stderr, "erreur d'envoie : %s\n", SDLNet_GetError() );
        } else {
            SDL_Delay( 1 );
            // TCPsocket client = SDLNet_TCP_Accept( serveur );
            // while (!client) client = SDLNet_TCP_Accept( serveur );
            // int foo;
            // if (!SDLNet_TCP_Recv( serveur, &foo, sizeof( foo ) ))
            //     fprintf( stderr, "erreur de reception : %s\n", SDLNet_GetError() );
            // SDLNet_TCP_Close( client );
        }
        SDLNet_TCP_Close( serveur );
    }
}

uint8_t toKey( SDL_Scancode  code ) {
    switch (code) {
    case SDL_SCANCODE_A ... SDL_SCANCODE_Z: return 'a' + code - SDL_SCANCODE_A;
        // case SDL_SCANCODE_GRAVE : return 'œ'
    case SDL_SCANCODE_1 ... SDL_SCANCODE_9: return 49 + code - SDL_SCANCODE_1;
    case SDL_SCANCODE_0: return 48;
    case SDL_SCANCODE_RETURN ... SDL_SCANCODE_TAB: return 0xB0 + code - SDL_SCANCODE_RETURN;
    case SDL_SCANCODE_SPACE: return ' ';
        // 45 nothing
    case SDL_SCANCODE_EQUALS: return '=';
    case SDL_SCANCODE_LEFTBRACKET: return '['; // ^
        // case SDL_SCANCODE_LEFTBRACKET: return '^'; // ^
    case SDL_SCANCODE_RIGHTBRACKET: return ']'; // $
    case SDL_SCANCODE_BACKSLASH: return '\\';  // *
    case SDL_SCANCODE_SEMICOLON: return ';'; // m
    case SDL_SCANCODE_APOSTROPHE: return '\''; // ù
    case SDL_SCANCODE_GRAVE: return '`'; // œ
    case SDL_SCANCODE_COMMA: return ','; // ;
    case SDL_SCANCODE_PERIOD: return '.'; // :
    case SDL_SCANCODE_SLASH: return '/'; // !
    case SDL_SCANCODE_CAPSLOCK: return 0xC1;
    case SDL_SCANCODE_F1 ... SDL_SCANCODE_PAUSE: return 0xC2 + code - SDL_SCANCODE_F1;
    case SDL_SCANCODE_INSERT ... SDL_SCANCODE_PAGEDOWN: return 0xD1 + code - SDL_SCANCODE_INSERT;
    case SDL_SCANCODE_RIGHT ... SDL_SCANCODE_UP: return 0xD7 + code - SDL_SCANCODE_RIGHT;
    case SDL_SCANCODE_NONUSBACKSLASH: return '<';
    case SDL_SCANCODE_LCTRL ... SDL_SCANCODE_RGUI: return 0x80 + code - SDL_SCANCODE_LCTRL;
    default: return code;
    }
}

t_event move = { 0, 0, 0, 0, 255 };
void updateInputs( SDL_Window* window, IPaddress ip, bool refreshMouse ) {
    SDL_Event e;
    t_event event = { 0, 0, 0, 0, 255 };
    while (SDL_PollEvent( &e )) {
        switch (e.type) {
        case SDL_QUIT: quit = true; break;
        case SDL_KEYUP:
            // if (e.key.keysym.sym == SDLK_F4) quit = false;
            event.action = KEY_UP;
            event.code = toKey( e.key.keysym.scancode );
            send( ip, event );
            break;
        case SDL_KEYDOWN:
            // if (e.key.keysym.sym == SDLK_F4) quit = false;
            event.action = KEY_DOWN;
            event.code = toKey( e.key.keysym.scancode );
            send( ip, event );
            break;
            // case SDL_MOUSEMOTION:
            //     move.action = MOUSE_MOVE;
            //     move.x = e.motion.x;
            //     move.y = e.motion.y;
            //     break;
            // case SDL_MOUSEBUTTONDOWN:
            //     event.action = MOUSE_DOWN;
            //     event.code = e.button.button;
            //     // fprintf( stdout, "mouse  button : %d \n", e.button.button );
            //     send( ip, event );
            //     break;
            // case SDL_MOUSEBUTTONUP:
            //     event.action = MOUSE_UP;
            //     event.code = e.button.button;
            //     send( ip, event );
            //     break;
                // case SDL_WINDOWEVENT:
                //     if (e.window.event == SDL_WINDOWEVENT_TAKE_FOCUS) {
                //         fprintf( stdout, "center : %d \n", e.window.event );
                //         SDL_WgfarpMouseInWindow( window, 2560, 0 );
                //         event.action = FOCUS;
                //         send( ip, event );
                //     }
                //     break;
        default:
            // fprintf( stdout, "event type : %d \n", e.type );
            break;
        }
    }
    if (move.action == MOUSE_MOVE && refreshMouse) {
        fprintf( stdout, "mouse  mouse : %d %d \n", move.x, move.y );
        send( ip, move );
        move.action = 0;
    }
}

int main( int argc, char const* argv[] ) {
    fprintf( stdout, "packet size : %d \n", sizeof( t_event ) );

    SDL_Window* window = init();
    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    IPaddress ip = initNetwork();
    int frame = 0;
    while (!quit) {
        SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
        SDL_RenderClear( renderer );
        SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
        //TODO
        SDL_RenderPresent( renderer );
        updateInputs( window, ip, frame == 0 );
        if (frame++ == 12) {
            frame = 0;
        }
    }
    SDL_DestroyWindow( window );
    SDL_Quit();
    return 0;
}