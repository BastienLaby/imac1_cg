#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* Dimensions de la fenêtre */
static unsigned int WINDOW_WIDTH = 800;
static unsigned int WINDOW_HEIGHT = 800;

/* Nombre de bits par pixel de la fenêtre */
static const unsigned int BIT_PER_PIXEL = 32;

/* Nombre minimal de millisecondes separant le rendu de deux images */
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

typedef struct Point {
    float x, y;
    unsigned char r, g, b;
    struct Point* next;
} Point, *PointList;

Point* allocPoint(float x, float y, unsigned char r, unsigned char g, unsigned char b) {
    Point* point = (Point*) malloc(sizeof(Point));
    if (!point) {
        return NULL;
    }
    point->x = x;
    point->y = y;
    point->r = r;
    point->g = g;
    point->b = b;
    point->next = NULL;
    return point;
}

void addPointToList(Point* point, PointList* list) {
    assert(point);
    assert(list);
    if (*list == NULL) {
        /* Si le pointeur sur la liste est vide, on le modifie pour le faire pointer sur le point donné en paramètre */
        *list = point;
    } else {
        /* Sinon, on réapplique la même fonction mais sur le point suivant */
        addPointToList(point, &(*list)->next);
    }
}

void drawPoints(PointList list) {
    while(list) {
        glColor3ub(list->r, list->g, list->b);
        glVertex2f(list->x, list->y);
        list = list->next;
    }
}

void deletePoints(PointList* list) {
    assert(list);
    while (*list) {
        Point* next = (*list)->next;
        free(*list);
        *list = next;
    }
}

typedef struct Primitive{
    GLenum primitiveType;
    PointList points;
    struct Primitive* next;
} Primitive, *PrimitiveList;

Primitive* allocPrimitive(GLenum primitiveType) {
    Primitive* primitive = (Primitive*) malloc(sizeof(Primitive));
    if (!primitive) {
        return NULL;
    }
    primitive->primitiveType = primitiveType;
    primitive->points = NULL;
    primitive->next = NULL;
    return primitive;
}

void addPrimitive(Primitive* primitive, PrimitiveList* list) {
    assert(primitive);
    assert(list);
    if (*list == NULL) {
        // Si le pointeur sur la liste est vide, on le modifie pour le faire pointer sur la primitive donnée en paramètre
        *list = primitive;
    } else {
        // Sinon, on réapplique la même fonction mais sur le point suivant
        addPrimitive(primitive, &(*list)->next);
    }
}

void drawPrimitives(PrimitiveList list) {
    while(list) {
        glBegin(list->primitiveType);
        drawPoints(list->points);
        glEnd();
        list = list->next;
    }
}

void deletePrimitive(PrimitiveList* list) {
    assert(list);
    while(*list) {
        Primitive* next = (*list)->next;
        deletePoints(&(*list)->points);
        free(*list);
        *list = next;
    }
}

void resizeViewport() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1., 1., -1., 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE);
}


static const unsigned char COLORS[] = {
    255, 255, 255,
    0, 0, 0,
    255, 0, 0,
    0, 255, 0,
    0, 0, 255,
    255, 255, 0,
    0, 255, 255,
    255, 0, 255
};

static const unsigned int NB_COLORS = sizeof(COLORS) / (3 * sizeof(unsigned char));

void drawColorSelectionView() {
    int i;
    GLfloat dx = 2.f / NB_COLORS;
    glBegin(GL_QUADS);
    for(i = 0; i < NB_COLORS; ++i) {
        glColor3ubv(COLORS + i * 3);
        glVertex2f(-1 + i * dx, -1);
        glVertex2f(-1 + (i + 1) * dx, -1);
        glVertex2f(-1 + (i + 1) * dx, 1);
        glVertex2f(-1 + i  * dx, 1);
    }
    glEnd();
}

int main(int argc, char** argv) {

    /* Initialisation de la SDL */
    if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    
    /* Ouverture d'une fenêtre et création d'un contexte OpenGL */
    if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE)) {
        fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    SDL_WM_SetCaption("Paint IMAC", NULL);

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    PrimitiveList primitives = NULL;
    Primitive* prim = allocPrimitive(GL_POINTS);
    PrimitiveList lastPrim = prim;
    addPrimitive(prim, &primitives);

    /* Boucle d'affichage */
    int loop = 1;
    int mode = 0;
    unsigned int currentColor = 0; // color index

    while(loop) {

        /* Récupération du temps au début de la boucle */
        Uint32 startTime = SDL_GetTicks();
        
        /* Code de dessin */

        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0.8, 0.0, 0.0);

        glPushMatrix();

            glTranslatef(0.0, 0.5, 0.0);
            glScalef(0.2, 0.2, 1.0);

            glBegin(GL_TRIANGLES);
                glVertex2f(-0.5, -0.5);
                glVertex2f(-0.5, 0.5);
                glVertex2f(0.5, 0.5);
                glVertex2f(0.5, 0.5);
                glVertex2f(-0.5, -0.5);
                glVertex2f(0.5, -0.5);
            glEnd();

        glPopMatrix();

        glPushMatrix();

            glTranslatef(0.0, -0.5, 0.0);
            glScalef(0.2, 0.2, 1.0);

            glBegin(GL_TRIANGLES);
                glVertex2f(-0.5, -0.5);
                glVertex2f(-0.5, 0.5);
                glVertex2f(0.5, 0.5);
                glVertex2f(0.5, 0.5);
                glVertex2f(-0.5, -0.5);
                glVertex2f(0.5, -0.5);
            glEnd();

        glPopMatrix();

        /*if (mode == 0) {
            drawPrimitives(primitives);
        }
        else if (mode == 1) {
            drawColorSelectionView();
        }*/

        glFlush();

        /* Boucle traitant les evenements */
        SDL_Event e;
        while(SDL_PollEvent(&e)) {

            /* L'utilisateur ferme la fenêtre : */
            if(e.type == SDL_QUIT) {
                loop = 0;
                break;
            }
            
            /* Quelques exemples de traitement d'evenements : */
            switch(e.type) {

                /* Clic souris */
                case SDL_MOUSEBUTTONUP:
                    if (mode == 1) {
                        currentColor = e.button.x * NB_COLORS / WINDOW_WIDTH;
                    }
                    else if (mode == 0) {
                        float x = -1 + 2. * e.button.x / WINDOW_WIDTH;
                        float y = -(-1 + 2. * e.button.y / WINDOW_HEIGHT);
                        unsigned int r = COLORS[currentColor * 3];
                        unsigned int g = COLORS[currentColor * 3 + 1];
                        unsigned int b = COLORS[currentColor * 3 + 2];
                        addPointToList(allocPoint(x, y, r, g, b), &lastPrim->points);
                    }
                    break;

                /* Touche clavier */
                case SDL_KEYDOWN:

                    printf("touche pressée (code = %d)\n", e.key.keysym.sym);

                    if (e.key.keysym.sym == SDLK_SPACE) {
                        mode = 1;
                    }

                    switch(e.key.keysym.sym) {

                        case SDLK_q:
                            loop = 0;

                        case SDLK_SPACE:
                            mode = 1;
                            break;

                        case SDLK_p:
                            prim = allocPrimitive(GL_POINTS);
                            lastPrim = prim;
                            addPrimitive(prim, &primitives);
                            break;

                        case SDLK_l:
                            prim = allocPrimitive(GL_LINES);
                            lastPrim = prim;
                            addPrimitive(prim, &primitives);
                            break;

                        case SDLK_t:
                            prim = allocPrimitive(GL_TRIANGLES);
                            lastPrim = prim;
                            addPrimitive(prim, &primitives);
                            break;

                        default:
                            break;
                    }

                    break;

                case SDL_KEYUP:
                    if (e.key.keysym.sym == SDLK_SPACE) {
                        mode = 0;
                    }
                    break;

                case SDL_VIDEORESIZE:
                    WINDOW_WIDTH = e.resize.w;
                    WINDOW_HEIGHT = e.resize.h;
                    resizeViewport();

                default:
                    break;
            }
        }

        /* Echange du front et du back buffer : mise à jour de la fenêtre */
        SDL_GL_SwapBuffers();

        /* Calcul du temps écoulé */
        Uint32 elapsedTime = SDL_GetTicks() - startTime;

        /* Si trop peu de temps s'est écoulé, on met en pause le programme */
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }

    deletePrimitive(&primitives);

    /* Liberation des ressources associées à la SDL */ 
    SDL_Quit();

    return EXIT_SUCCESS;
}
