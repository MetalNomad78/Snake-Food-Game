#include <raylib.h>
#include<bits/stdc++.h>
#include<raymath.h>

using namespace std;


Color green={173,204,96,255};
Color darkGreen={43,51,24,255};

int cellSize=30;
int cellCount=25;
int offset=75;

double lastUpdateTime=0;

//to check if element in snake body

bool ElementInDeque(Vector2 element,deque<Vector2> deque){
    for(unsigned int i=0;i<deque.size();i++){
        if(Vector2Equals(deque[i],element)){
            return true;
        }
    }
    return false;
}
//to check if some interval has passed
bool eventTriggered(double interval){
    double currentTime=GetTime();
    //if this is true then something has happened and interval has happened
    if(currentTime-lastUpdateTime>=interval){
        lastUpdateTime=currentTime;
        return true;
    }
    return false;
}

//creating snake
class Snake{
    public:
    deque<Vector2> body={Vector2{6,9},Vector2{5,9},Vector2{4,9}};
    Vector2 direction={1,0};//knowing direction of the snake
    bool addSegment=false;//to check when to not move


    //drawing the snake
    void Draw(){
        //we need to draw each segment of the snake i.e. the 
        //cells of snakes therefore we use for loop on deque
        for(unsigned int i=0;i<body.size();i++){
            float x=body[i].x;
            float y=body[i].y;

            //to make it more pleasing and look good on the eye
            //needs(rectangle object,roundness,no. of line segments used to draw each corner,color)
            Rectangle segment=Rectangle{offset + x*cellSize, offset + y*cellSize,(float)cellSize,(float)cellSize};
            DrawRectangleRounded(segment,0.5,6,darkGreen);
        }
    }

    //updating the snake
    void Update(){
        body.push_front(Vector2Add(body[0],direction));//adding one to the head
        //if true we do not move the snake and just add one at the beginning
        if(addSegment){
            addSegment=false;
        }else{
            body.pop_back();//removing one from behind
        
        }
        
    }
    void Reset(){
        body={Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        direction={1,0};
    }
};

//creating food
class Food{
    public:
    Vector2 position;
    Texture2D texture;//creating a 2d renderable texture of the image
    
    //creating constructor
    Food(deque<Vector2> snakeBody){
        Image image=LoadImage("graphics/apple.png");
        texture=LoadTextureFromImage(image);
        UnloadImage(image);
        position=GenerateRandomPos(snakeBody);
    }

    //unloading food
    ~Food(){
        UnloadTexture(texture);
    }

    //drawing food
    void Draw(){
        DrawTexture(texture,offset + position.x*cellSize,offset + position.y*cellSize,WHITE);//white is just the outer color of the fruit
    }

    //just to make it a good practice otherwise we can copy the lines everywhere as well
    Vector2 GenerateRandomCell(){
        float x=GetRandomValue(0,cellCount-1);
        float y=GetRandomValue(0,cellCount-1);
        return Vector2{x,y};
    }
    //randomizing the food placement and also keep a check that food doesnt spawn in snake body
    Vector2 GenerateRandomPos(deque<Vector2> snakeBody){      
        Vector2 position =GenerateRandomCell();
        while(ElementInDeque(position,snakeBody)){
            //if element in body of snake generate again
            position=GenerateRandomCell();
        }
        return position;
    }
};
class Game{
    public:
    Snake snake=Snake();
    Food food=Food(snake.body);
    bool running=true;
    int score=0;
    Sound eatSound;
    Sound wallSound;
    

    //constructor for game class
    Game(){
        InitAudioDevice();//initializes audio device for the game and setup the system for the game
        eatSound=LoadSound("sounds/eat.mp3");
        wallSound=LoadSound("sounds/wall1.wav");
    }
    ~Game(){
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }


    void Draw(){
        food.Draw();
        snake.Draw();
    }
    void Update(){
        if(running){
        snake.Update();
        CheckCollisionWithFood();
        CheckCollisionWithWall();
        CheckCollisonWithTail();
        }
    }

    void CheckCollisionWithFood(){
        if(Vector2Equals(snake.body[0],food.position)){
            //to generate new position when we eat food
            food.position=food.GenerateRandomPos(snake.body);
            snake.addSegment=true;
            score++;//adding scores
            PlaySound(eatSound);
        }
    }
    void CheckCollisionWithWall(){
        if(snake.body[0].x==cellCount||snake.body[0].x==-1){
            GameOver();
        }
        if(snake.body[0].y==cellCount||snake.body[0].y==-1){
            GameOver();
        }
        
    }
    void CheckCollisonWithTail(){
        deque<Vector2> headlessBody=snake.body;
        headlessBody.pop_front();//remove head from the body
        if(ElementInDeque(snake.body[0],headlessBody)){
            GameOver();
        }


    }
    void GameOver(){
            snake.Reset();//bring back to original position
            food.position=food.GenerateRandomPos(snake.body);
            running=false;//to pause
            score=0;
            
            
            PlaySound(wallSound);
            
        }
};
int main()
{
    InitWindow(2*offset+cellSize*cellCount,2*offset+cellSize*cellCount,"Snake.io");//initializing the window

    SetTargetFPS(60);//setting game limit otherwise it will run on maximizing performance
    //game loop


    Game game=Game();

    while(WindowShouldClose()==false){
        
        
        BeginDrawing();
        

        //to slow down the process of snake moving and not move it to fast as before
        //due to high fps
        if(eventTriggered(0.2)){
            game.Update();
        }
        if(game.running==false){
            
        int textWidth = MeasureText("GAME OVER", 40);
        int textX = (GetScreenWidth() - textWidth) / 2;
        int textY = GetScreenHeight() / 2 - 20;
        int textWidth1 = MeasureText("PRESS ANY 'ARROW KEY' TO PLAY AGAIN", 20);
        DrawText("GAME OVER", textX, textY, 40, darkGreen);
        DrawText("PRESS ANY 'ARROW KEY' TO PLAY AGAIN", (GetScreenWidth() - textWidth1) / 2, textY+50, 20, darkGreen);
        }
        //taking input
        if(IsKeyPressed(KEY_UP)&&game.snake.direction.y != 1){
            game.snake.direction={0,-1};
            game.running=true;//y increases downwards and reduces upwards
            
        }
        if(IsKeyPressed(KEY_DOWN)&&game.snake.direction.y != -1){
            game.snake.direction={0,1};
            game.running=true;//y increases downwards and reduces upwards
            
        }
        if(IsKeyPressed(KEY_RIGHT)&& game.snake.direction.x != -1){
            game.snake.direction={1,0};
            game.running=true;//y increases downwards and reduces upwards
            
        }
        if(IsKeyPressed(KEY_LEFT)&&game.snake.direction.x != 1){
            game.snake.direction={-1,0};
            game.running=true;//y increases downwards and reduces upwards
            
        }
        

        ClearBackground(green);

        //creating border of 5 px thickness so start from offset -5 and the width adding both thickness
        //becomes 10 as 5 thickness on left and right therefore total width 10 and similar for height
        DrawRectangleLinesEx(Rectangle{float(offset-5),float(offset-5),float(cellSize*cellCount+10),float(cellSize*cellCount+10)},5,darkGreen);

        DrawText("Snake.io",offset-5,20,40,darkGreen);
        DrawText(TextFormat("%i",game.score),offset-5,offset+cellSize*cellCount+10,40,darkGreen);
        
        game.Draw();

        EndDrawing();
    }
    CloseWindow();//destroying window

    return 0;
}