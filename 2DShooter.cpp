
// 2D top down shooter game

//Compile as: g++ 2DShooter.cpp -lncurses


//#include<iostream>    //cin/cout NOT USED. DO NOT ENABLE!
#include<fstream>       //File Streams
#include<ncurses.h>     //N Curses Library
#include<stdlib.h>      //srand
#include<time.h>        //time
#include<vector>
#include<string>

using namespace std;

void initCurses();
void endCurses();
//---------------------------------------------------------------------
// node for queue for pathfinding
struct node{
  node* next;
  int info;
  node( int newInfo){
    next = NULL;
    info = newInfo;
  }
};
// queue for pathfinding
struct queue {
  queue(){
    front = NULL;
    back = NULL;
  }
  void pushQ ( int newInfo ){
    node * newNode = new node(newInfo);
    if(back== NULL){
      front = newNode;
      back = newNode;
      return;
    }
    back->next = newNode;
    back = newNode;
  }
  int popQ(){
    int popInfo = front-> info;
    node* temp = front;
    front = front -> next;
    if(front == NULL){
      back = NULL;
    }
    delete temp;
    return popInfo;
  }
  bool isEmpty(){
    if(front==NULL){
      return TRUE;
    }
    else{
      return FALSE;
    }
  }
  node* front;
  node* back;
};
//---------------------------------------------------------------------
// master class that encapsulates player, bomb, and projectile
class entityType{
    public:
        entityType():x(0), y(0){}
        int x, y;
        virtual char whatamI(){
            return 'e';
        }
        entityType( int newX, int newY ){
          x = newX;
          y = newY;
        }
        void setCoordinates(int newX, int newY){
            x = newX;
            y = newY;
        }
};
//---------------------------------------------------------------------
class bombType;
class AIDispatcher;
// player class
class playerType : public entityType {
  friend class pointType;
  friend class bombType;
  public:
    bool isHuman;
    bombType * bomb;
    bool alive;
    char team;
    char lastDirection;

    playerType( char type, int newX, int newY ) : entityType(newX, newY){
      team = type;
      isHuman = TRUE;
      bomb = NULL;
      alive = TRUE;
    }
    playerType(char type, int newX, int newY, bool bot):entityType(newX,newY){
      team = type;
      isHuman = bot;
      bomb = NULL;
      alive = TRUE;
    }
    char whatamI(){
      if(isHuman == TRUE){
        return 'p';
      }
      else if(isHuman ==  FALSE && team == 't')
        return 'T';
      else if(isHuman == FALSE && team == 'c')
        return 'C';
    }

};
//---------------------------------------------------------------------
// bomb class
class bombType : public entityType{
  public:
    bool isPlanted;
    bool isCarried;
    bool isDefused;
    bombType( int newX, int newY ) : entityType(newX, newY){
      bool isPlanted = FALSE;
      bool isCarried = FALSE;
      bool isDefused = FALSE;
    }
    char whatamI(){
      return 'b';
    }
};
//---------------------------------------------------------------------
// projectile class
class projType : public entityType{
public:
  char direction;
  playerType * owner;
  projType(char whichWay, int x, int y, playerType * p) : entityType(x, y){
    direction = whichWay;
    owner = p;
  }
  char whatamI(){
    return '*';
  }
};
//---------------------------------------------------------------------
// map class
class charMap {
  public:
    charMap(char *arg);
    charMap(char** c, string m, int w, int h) :
        map(c), mapName(m), width(w), height(w){}
    ~charMap();
    void print();
    char ** map;
    string mapName;
    int width;
    int height;
};
//---------------------------------------------------------------------
// 1x1 point on map class
class pointType {
public:
    char baseType;
    vector<entityType * > entList;
    bool isBombsite;
    bool isObstacle;
    bool isWall;
    bool isBridgeTunnel;
    int x, y;
    bombType * newBomb;
    pointType(){}
    ~pointType(){

    }
    pointType(char mapPoint, int newX, int newY ){
      baseType = mapPoint;
      x = newX;
      y = newY;
      isBombsite = FALSE;
      isObstacle = FALSE;
      isWall = FALSE;
      isBridgeTunnel = FALSE;
      // point is a wall
      if(mapPoint == 'x'){
        isWall = TRUE;

      }
      else if(mapPoint == '#'){
        baseType = ' ';
        isBridgeTunnel = TRUE;
      }
      else if(mapPoint == 'o'){
        isObstacle = TRUE;
      }
      else if(mapPoint == 'B'){
        baseType = ' ';
        entList.push_back(new bombType(newX,newY));
      }
      else if(mapPoint == 'P' || mapPoint == '1' || mapPoint == '2'
            || mapPoint == '3' ){
        baseType = ' ';
        isBombsite = TRUE;
      }
      else if( mapPoint == 'C' || mapPoint == 'T'){
        baseType = ' ';
      }
    }
    // render by priority
    void renderPoint(){
      // render all the x, n, o, and ' '
      char render = this -> baseType;

      // now render the P's
      if( this->isBombsite ){
        render = 'P';

      }

      // render tunnel tunnel bridge
      if(this->isBridgeTunnel){
        render = '#';
      }

      // check entList to print bomb
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        // render the bomb
        if((entList[i])->whatamI() == 'b'){
          render = 'B';
          break;
        }
      }

      // check to print projectiles
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        // render the projectiles
        if((entList[i])->whatamI() == '*'){
          render = '*';
          break;
        }
      }

      // check to print  counter terrorist bots
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        // render the bots
        if((entList[i])->whatamI() == 'C'){
          if(entList[i] == NULL){
            continue;
          }
          render = 'C';
          break;
        }
      }

      // check to print terrorist bots
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        // render the bots
        if((entList[i])->whatamI() == 'T'){
          render = 'T';
          break;
        }
      }

      // check to print player
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        // render the player
        if((entList[i])->whatamI() == 'p'){
          render = '@';
          break;
        }
      }
      //refresh();
      printw("%c", render);

    }
    void initSpawn( playerType * player){
      entList.push_back(player);
      player->lastDirection = 'u';
    }
    // delete an entity from vector
    void deleteEntFromPoint(entityType* entity){
      // search and delete
      for(int i =0; i< entList.size() ; i++){
        if(entList[i] == NULL){
          continue;
        }
        if(entList[i]->whatamI() == entity->whatamI()){
          entList[i] = NULL;
          entList.erase(entList.begin()+i);
          break;
        }
      }
    }
    // what happens when player die
    void RIP(playerType * player){
      player->alive = FALSE;
      // if player had bomb
      if(player->bomb){
        this->entList.push_back(player->bomb);
        player->bomb->isCarried = FALSE;
      }
      this->deleteEntFromPoint(player);
    }
};
//---------------------------------------------------------------------
// map engine class
class mapEngine{
  public:
    friend class pointType;
    friend class entityType;
    vector<vector<pointType> > points;
    playerType * newPlayer;
    playerType * newBot;
    int height;
    int width;
    charMap* mapref;
    int roundTimer;
    int bombTimer;
    bool bombPlanted;
    bool bombDefused;
    int tAlive;
    int cAlive;
    int xT, yT;
    int xC, yC;
    mapEngine(){}
    ~mapEngine(){
      //delete newPlayer; newPlayer = NULL;
    }
    // determine game condition
    void endCondition( int condition ) {
      if(condition == 0){
        return;
      }
      if(condition == 1){
        printw("\nBOMB DEFUSED - CT WIN \n");

      }
      if(condition == 2){
        refresh();
        printw("\n BOMB EXPLODED - T WIN\n");


      }
      if(condition == 3){
        printw("\n CT DEAD - T WIN\n");

      }
      if(condition == 4){
        printw("\n T DEAD AND BOMB NOT PLANTED - CT WIN\n");
      }
      if(condition == 5){
        printw("\n TIME IS UP AND BOMB NOT PLANTED - CT WIN\n");
      }
    }
    // initialize map and game stats
    mapEngine( charMap *map ){
      mapref = map;
      height = map->height;
      width = map->width;
      // adjust vector size to map dimensions
      points.resize(height, vector<pointType>(width));
        // create new vector for all points of map
        for(int i=0; i < height; i++){
          for(int j=0; j < width; j++){
            if(map->map[i][j] == 'C'){
              xC = i;
              yC = j;
            }
            else if(map->map[i][j] == 'T'){
              xT = i;
              yT = j;
            }
            points[i][j] = pointType(map->map[i][j],i,j);
          }
        }
        roundTimer = 300;
        bombTimer = 80;
        bombPlanted = FALSE;
        bombDefused = FALSE;
        tAlive = 5;
        cAlive = 5;
    }
    // render the map
    void renderMap(){
      for(int i=0; i< height; i++){
        for(int j=0; j< width; j++){
            points[i][j].renderPoint();
        }
        printw("\n");
      }
    }
    void clearScreen(){
        refresh();
        clear();
        if(bombPlanted){
            printw("Time remaining: %d \n" , bombTimer);
            printw("Bomb planted! \n");
            bombTick();
        } else{
            printw("Time remaining: %d \n" , roundTimer);
            printw("Bomb not planted \n");
            secondTick();
        }
        checkRoundStatus();
    }
    playerType *userMenu(){
      char userInput;

      printw("Enter C for Counter-Terrorist or enter T for Terrorist Team \n");
      userInput = getch();
      // error check
      while(userInput != 'T' && userInput != 't' && userInput != 'c'
          && userInput != 'C'){
        printw("Bad input, try again\n");
        printw("Enter C for Counter-Terrorist or enter T for Terrorist Team \n");
        userInput = getch();
      }
      if(userInput == 't' || userInput == 'T'){
        printw("You are playing as terrorist \n");
        newPlayer = new playerType('t', xT, yT);


        points[xT][yT].initSpawn(newPlayer);
        // spawn all enemy bots
        int iteration = 0;
        while(iteration <5){
        newBot = new playerType('c', xC, yC, FALSE);
        points[xC][yC].initSpawn(newBot);
        iteration++;
        }

        // spawn player team
        iteration = 0;
        while(iteration < 4){
        newBot = new playerType('t', xT, yT, FALSE);
        points[xT][yT].initSpawn(newBot);
        iteration++;
        }

        renderMap();
        return newPlayer;
      }
      else if(userInput == 'c' || userInput =='C'){
        printw("You are playing as counterterrorist \n");
        newPlayer = new playerType('c', xC, yC);
        newBot = new playerType('t', xT, yT, FALSE);

        points[xC][yC].initSpawn(newPlayer);
        // spawn enemy team
        points[xT][yT].initSpawn(newBot);
        newBot = new playerType('t', xT, yT, FALSE);
        points[xT][yT].initSpawn(newBot);
        newBot = new playerType('t', xT, yT, FALSE);
        points[xT][yT].initSpawn(newBot);
        newBot = new playerType('t', xT, yT, FALSE);
        points[xT][yT].initSpawn(newBot);
        newBot = new playerType('t', xT, yT, FALSE);
        points[xT][yT].initSpawn(newBot);

        // spawn player team
        newBot = new playerType('c', xC, yC, FALSE);
        points[xC][yC].initSpawn(newBot);
        newBot = new playerType('c', xC, yC, FALSE);
        points[xC][yC].initSpawn(newBot);
        newBot = new playerType('c', xC, yC, FALSE);
        points[xC][yC].initSpawn(newBot);
        newBot = new playerType('c', xC, yC, FALSE);
        points[xC][yC].initSpawn(newBot);

        // render map
        renderMap();
        return newPlayer;
      }
    }
    void secondTick(){
        roundTimer--;
    }
    void bombTick(){
        bombTimer--;
    }
    int checkRoundStatus(){
        if( bombDefused && bombTimer != -1){
          endCondition(1);
          return 1;
        }
        else if (bombPlanted && bombTimer == -1){
            endCondition(2);
            return 2;

      } else if (cAlive == 0){
          endCondition(3);
          return 3;
        }else if (tAlive == 0 && !bombPlanted){
            endCondition(4);
            return 4;
        } else if (!bombPlanted && roundTimer == -1){
            endCondition(5);
            return 5;

        }
        return 0;

    }

};
//---------------------------------------------------------------------
// projectile class
class projEngine{
public:
  vector<projType*> projList;
  mapEngine * levelref;
  projEngine( mapEngine * level ){
      levelref = level;
  }
  // add a projectile
  void addProj(projType* proj){
    projList.push_back(proj);
  }
  // update all projectiles on map
  void updateAll(){

    for(int i =0; i < projList.size(); i++){
      int x = projList[i]->x;
      int y = projList[i]->y;
      // update coordinate
      // move up
      if(projList[i]->direction == 'u'){
        projList[i]->setCoordinates(x-1, y);
      }
      // move left
      else if(projList[i]->direction == 'l'){
        projList[i]->setCoordinates(x, y-1);
      }
      // move right
      else if(projList[i]->direction == 'r'){
        projList[i]->setCoordinates(x, y+1);
      }
      // move down
      else if(projList[i]->direction == 'd'){
        projList[i]->setCoordinates(x+1, y);

      }
      x = projList[i]->x;
      y = projList[i]->y;
      // if projectile hits wall
      if(levelref->points[x][y].baseType == 'x'){
        // remove from point
        levelref->points[x][y].deleteEntFromPoint(projList[i]);
        // determine where the previous point's position was and remove
        if(projList[i]->direction == 'u'){
          levelref->points[x+1][y].deleteEntFromPoint(projList[i]);
        }
        else if(projList[i]->direction == 'l'){
          levelref->points[x][y+1].deleteEntFromPoint(projList[i]);
        }
        else if(projList[i]->direction =='r'){
          levelref->points[x][y-1].deleteEntFromPoint(projList[i]);
        }
        else if(projList[i]-> direction == 'd'){
          levelref->points[x-1][y].deleteEntFromPoint(projList[i]);
        }
        delete projList[i];
        projList[i] = NULL;
        projList.erase(projList.begin()+i);
        continue;
      }
      // check if there's entities in pointType
      else if(levelref->points[x][y].entList.empty() ){
        // add to that point's entity list
        levelref->points[x][y].entList.push_back(projList[i]);

      }
      // there are entities
      else if( levelref->points[x][y].entList.size() > 0){
        for(int i =0; i < levelref->points[x][y].entList.size() ; i++){
          if(levelref->points[x][y].entList[i]->whatamI()=='T'
          || levelref->points[x][y].entList[i]->whatamI()=='C' ||
        levelref->points[x][y].entList[i]->whatamI()=='p'){
            // check if entity is owner of projectile
            if(projList[i]->owner == levelref->points[x][y].entList[i]){
              continue;
            }
            // change alive count
            else if(dynamic_cast<playerType*>(levelref->points[x][y].entList[i])->team=='c'){
              levelref->cAlive = levelref->cAlive - 1;
            }
            else{
              levelref->tAlive = levelref->tAlive -1 ;
            }
              // delete dead player
              levelref->points[x][y].RIP(dynamic_cast<playerType*>(levelref->points[x][y].entList[i]));

          }
        }
      }
      // determine where the previous point's position was and remove
      if(projList[i]->direction == 'u'){
        levelref->points[x+1][y].deleteEntFromPoint(projList[i]);
      }
      else if(projList[i]->direction == 'l'){
        levelref->points[x][y+1].deleteEntFromPoint(projList[i]);
      }
      else if(projList[i]->direction =='r'){
        levelref->points[x][y-1].deleteEntFromPoint(projList[i]);
      }
      else if(projList[i]-> direction == 'd'){
        levelref->points[x-1][y].deleteEntFromPoint(projList[i]);
      }
    }
  }

};
//---------------------------------------------------------------------
// movement class
class movement{
public:
  static char readkeyInput(){
    int ch= getch();
    switch(ch){
      // MOVE UP
      case 'w':
      case 'W':
      case KEY_UP:
          return 'w';
      // MOVE DOWN
      case 's':
      case 'S':
      case KEY_DOWN:
          return 's';
      // MOVE LEFT
      case 'a':
      case 'A':
      case KEY_LEFT:
          return 'a';
      // MOVE RIGHT
      case 'd':
      case 'D':
      case KEY_RIGHT:
          return 'd';
      // IDLE KEY
      case 'i':
      case 'I':
          return 'i';
      // EXIT GAME
      case 'q':
      case 'Q':
          return 'q';
      // refresh window
      case 'c':
      case 'C':
          return 'c';
      // SPACE BAR
      case ' ':
          return ' ';
      default:
        readkeyInput();
        break;
    }
  }
  static void makeMove(mapEngine *level, playerType *player, char direction, projEngine * proj){
    if(player->alive == FALSE){
      return;
    }
    int x = player->x;
    int y = player->y;
    // refresh
    if(direction == 'c'){
      refresh();
      return;
    }
    // idle
    if(direction == 'i'){
      return;
    }
    // shoot
    if( direction == ' '){
      projType * newProj = new projType(player->lastDirection, x,y,player);
      proj->addProj(newProj);
      level->points[x][y].entList.push_back(newProj);
    }
    // MOVE UP
    if(direction == 'w'){
        if(level->points[x-1][y].baseType == ' ' &&
            level->points[x][y].isBridgeTunnel &&
            (player->lastDirection=='u' ||player->lastDirection=='d' )){

          level->points[x-1][y].entList.push_back(player);
          level->points[x][y].deleteEntFromPoint(player);
          player->setCoordinates(x-1, y);
          player->lastDirection = 'u';
            }
            else if(level->points[x-1][y].baseType == ' ' &&
                level->points[x][y].isBridgeTunnel &&
          (player->lastDirection!='u' ||player->lastDirection!='d' )){
            return;
          }
        else if(level->points[x-1][y].baseType == ' ' &&
            !level->points[x][y].isBridgeTunnel){
        level->points[x-1][y].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x-1, y);
        player->lastDirection = 'u';
      }
      else{
        player->lastDirection = 'u';
        return;
      }
    }
    // MOVE LEFT
    if(direction == 'a'){
      if(level->points[x][y-1].baseType == ' ' &&
          level->points[x][y].isBridgeTunnel &&
          (player->lastDirection=='r' ||player->lastDirection=='l' )){
        level->points[x][y-1].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x, y-1);
        player->lastDirection = 'l';
          }
          else if(level->points[x][y-1].baseType == ' ' &&
              level->points[x][y].isBridgeTunnel &&
          (player->lastDirection!='r' ||player->lastDirection!='l' )){
            return;
          }
      else if(level->points[x][y-1].baseType == ' ' &&
          !level->points[x][y].isBridgeTunnel){
        level->points[x][y-1].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x, y-1);
        player->lastDirection = 'l';
      }
      else{
        player->lastDirection = 'l';
        return;
      }
    }
    // MOVE DOWN
    if(direction == 's'){
      if( level->points[x+1][y].baseType == ' ' &&
          level->points[x][y].isBridgeTunnel &&
          (player->lastDirection=='u' ||player->lastDirection=='d' )){
        level->points[x+1][y].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x+1, y);
        player->lastDirection = 'd';
          }
        else  if( level->points[x+1][y].baseType == ' ' &&
              level->points[x][y].isBridgeTunnel &&
        (player->lastDirection!='u' ||player->lastDirection!='d' )){
          return;
        }
      else if(level->points[x+1][y].baseType == ' ' &&
          !level->points[x][y].isBridgeTunnel){
        level->points[x+1][y].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x+1, y);
        player->lastDirection = 'd';
      }
      else{
        player->lastDirection = 'd';
        return;
      }
    }
    // MOVE RIGHT
    if(direction == 'd'){
      if(level->points[x][y+1].baseType == ' ' &&
          level->points[x][y].isBridgeTunnel &&
          (player->lastDirection=='r' ||player->lastDirection=='l' )){
        level->points[x][y+1].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x, y+1);
        player->lastDirection = 'r';
          }
      else if(level->points[x][y+1].baseType == ' ' &&
          level->points[x][y].isBridgeTunnel &&
        (player->lastDirection!='r' ||player->lastDirection!='l' )){
          return;
        }
      else if(level->points[x][y+1].baseType == ' ' &&
          !level->points[x][y].isBridgeTunnel){
        level->points[x][y+1].entList.push_back(player);
        level->points[x][y].deleteEntFromPoint(player);
        player->setCoordinates(x, y+1);
        player->lastDirection = 'r';
      }
      else{
        player->lastDirection = 'r';
        return;
      }
    }
    // after movement adjustments, do post checks
    postMoveChecks(level, player);

  }
  // post checks such as bomb carry, bomb plant, bomb defusal
  static void postMoveChecks(mapEngine*level, playerType*player){
    int x = player->x;
    int y = player->y;
    // check if player can pick up bomb
    if(player->team == 't'){
      for(int i=0; i < level->points[x][y].entList.size(); i++){
        if(level->points[x][y].entList[i]->whatamI() == 'b'){
          player->bomb = dynamic_cast<bombType*>(level->points[x][y].entList[i]);
          player->bomb->isCarried = TRUE;
          level->points[x][y].deleteEntFromPoint(level->points[x][y].entList[i]);
        }
      }
    }
    // check if player can plant bomb
    if(player->team == 't' && (player->bomb)){
        if(level->points[x][y].isBombsite){
          level->points[x][y].entList.push_back(player->bomb);
          player->bomb->isCarried = FALSE;
          player->bomb->isPlanted = TRUE;
          player->bomb->setCoordinates(x,y);
          player->bomb = NULL;
          // update bomb endCondition
          level->bombPlanted = TRUE;
          printw("BOMB HAS BEEN PLANTED");
          //getch();
        }
    }
    // check if player can defuse bomb
    if(player->team == 'c' && level->points[x][y].isBombsite){
      for(int i=0; i < level->points[x][y].entList.size(); i++){
        if(level->points[x][y].entList[i]->whatamI() == 'b'){
          dynamic_cast<bombType*>(level->points[x][y].entList[i])->isCarried = FALSE;
          dynamic_cast<bombType*>(level->points[x][y].entList[i])->isDefused = TRUE;
          level->bombDefused = TRUE;
        }
      }
    }
  }
};

class AItype{
    public:
      friend class movement;
      vector<playerType*> botList;
      playerType* human;
      bombType* bomb;
      mapEngine* levelref;
      int totalBots;
      projEngine* ballref;
      int bombsiteGPS;
      int numSites;
      bool targetSite;
      int numVerts;
      vector<vector<int>> aList;
      vector<int> pathtoSite;
      vector<int> siteOpts;
      // BFS used for finding path
      bool breadSearch(vector<vector<int>> a_List, int start, int end, int numVerts, int pastV[]){

          queue bfsQueue;
          //check which vertices have been visited
          bool visited[numVerts];

          for (int i = 0; i < numVerts; i++){
              visited[i] = false;
              pastV[i] = -1;
          }

          //  first vertex is visited
          visited[start] = true;
          bfsQueue.pushQ(start);
          // check all paths from vertex
          while (!bfsQueue.isEmpty()){
              int curr = bfsQueue.popQ();
              int iteration = 0;
              while(iteration < a_List[curr].size()){
                  // evaluate all neighbor vertices and update list with prev
                  if(visited[a_List[curr][iteration]] == false){
                      visited[a_List[curr][iteration]] = true;
                      pastV[a_List[curr][iteration]] = curr;
                      bfsQueue.pushQ(a_List[curr][iteration]);

                      // succesfully find end
                      if(a_List[curr][iteration] == end)
                         return true;
                  }
                  iteration++;
              }
          }

          // no path found
          return false;
          }


    void addHuman(playerType *humanFound){
        human = humanFound;
        return;
    }
    void addBot(playerType *botFound){
        botList.push_back(botFound);
        totalBots++;
    }
    void addBomb(bombType *bombFound){
        bomb = bombFound;
    }

    AItype(mapEngine *level, projEngine *proj){
        levelref = level;
        ballref = proj;
        totalBots = 0;
        for(int i = 0; i < level->height; i++)
            for(int j = 0; j < level->width; j++)
                for(int k = 0; k < level->points[i][j].entList.size(); k++){
                    if(level->points[i][j].entList[k]->whatamI() == 'T' ||
                    level->points[i][j].entList[k]->whatamI() ==  'C'){
                        addBot(dynamic_cast<playerType*>(level->points[i][j].entList[k]));
                    } else if (level->points[i][j].entList[k]->whatamI() == 'p'){
                        addHuman(dynamic_cast<playerType*>(level->points[i][j].entList[k]));
                    } else if (level->points[i][j].entList[k]->whatamI() == 'b'){
                        addBomb(dynamic_cast<bombType*>(level->points[i][j].entList[k]));
                    }
                }
            numSites = 0;
            numVerts = level->height * level->width;
            aList.resize(numVerts);
            for (int i = 0; i < numVerts; i++){
                int currX = i / level->width;
                int currY = i % level->width;

                int upIndex = i - level->width;
                int downIndex = i + level->width;
                int rightIndex = i+1;
                int leftIndex = i-1;

                int upX = currX-1;
                int downX = currX+1;
                int rightY = currY+1;
                int leftY = currY-1;

                if (level->points[currX][currY].isBombsite){
                    siteOpts.push_back(currX * level->width + currY);
                    numSites++;
                }

                if (upX >= 0 && upX < level->height && upIndex >= 0  && level->points[currX][currY].baseType == ' ' && level->points[upX][currY].baseType == ' '){
                    aList[i].push_back(upIndex);
                }
                if (downX < level->height && downIndex < numVerts && level->points[currX][currY].baseType == ' ' && level->points[downX][currY].baseType == ' '){
                    aList[i].push_back(downIndex);
                }
                if (rightY < level->width && rightIndex < numVerts && level->points[currX][currY].baseType == ' ' && level->points[currX][rightY].baseType == ' '){
                    aList[i].push_back(rightIndex);
                }
                if (leftY >= 0 && leftY < level->width && leftIndex >= 0 && level->points[currX][currY].baseType == ' ' && level->points[currX][leftY].baseType == ' '){
                    aList[i].push_back(leftIndex);

                }

                }
    }

    void checkForNewDead(){
        for(int i = 0; i < botList.size(); i++){
            if(!(botList[i]->alive))
                botList[i] = NULL;
        }
    }

    void updateAll(){
            srand(time(NULL));
            int onLeft;
            int onRight;
            bool botShot;
            char rngMove[5] = {'i', 'w', 'a', 's', 'd'};
            int n = botList.size();
            //checkForNewDead();
            //  bot shooting mechanics
            for(int i = 0; i < n; i++){
                botShot = FALSE;
                if(human->alive && botList[i]->team != human->team && (botList[i]-> x == human->x || botList[i]-> y == human->y)){
                    botShot = TRUE;
                    if(botList[i]-> x == human->x){
                        if (botList[i]->y > human->y){
                            onLeft = human->y;
                            onRight = botList[i]->y;
                            botList[i]->lastDirection = 'l';
                        } else {
                            onLeft = botList[i]->y;
                            onRight = human->y;
                            botList[i]->lastDirection = 'r';
                        }
                        for(int j = onLeft + 1; j < onRight; j++){
                                if(levelref->points[human->x][j].isWall)
                                    botShot = FALSE;
                        }
                    } else {
                        if (botList[i]->x > human->x){
                            onLeft = human->x;
                            onRight = botList[i]->x;
                            botList[i]->lastDirection = 'u';
                        } else {
                            onLeft = botList[i]->x;
                            onRight = human->x;
                            botList[i]->lastDirection = 'd';
                        }
                        for(int j = onLeft + 1; j < onRight; j++){
                            if(levelref->points[j][human->y].isWall)
                                    botShot = FALSE;
                        }
                    }
                }
                    if(botShot){
                        movement::makeMove(levelref, botList[i], ' ', ballref);
                     }

            }
            // more bot shooting mechanics
            for(int i = 0; i < n - 1; i++){
                for (int j = i + 1; j < n; j++){
                botShot = FALSE;
                if(botList[i]->alive && botList[j]->team != botList[i]->team && (botList[j]-> x == botList[i]->x || botList[j]-> y == botList[i]->y)){
                    botShot = TRUE;
                    if(botList[j]-> x == botList[i]->x){
                        if (botList[j]->y > botList[i]->y){
                            onLeft = botList[i]->y;
                            onRight = botList[j]->y;
                            botList[j]->lastDirection = 'l';
                            botList[i]->lastDirection = 'r';
                        } else {
                            onLeft = botList[j]->y;
                            onRight = botList[i]->y;
                            botList[j]->lastDirection = 'r';
                            botList[i]->lastDirection = 'l';
                        }
                        for(int k = onLeft + 1; k < onRight; k++){
                                if(levelref->points[botList[i]->x][k].isWall)
                                    botShot = FALSE;
                                    break;
                        }
                    } else {
                        if (botList[j]->x > botList[i]->x){
                            onLeft = botList[i]->x;
                            onRight = botList[j]->x;
                            botList[j]->lastDirection = 'u';
                            botList[i]->lastDirection = 'd';
                        } else {
                            onLeft = botList[j]->x;
                            onRight = botList[i]->x;
                            botList[j]->lastDirection = 'd';
                            botList[i]->lastDirection = 'u';
                        }
                        for(int k = onLeft + 1; k < onRight; k++){
                            if(levelref->points[k][botList[i]->y].isWall)
                                    botShot = FALSE;
                                    break;
                        }
                    }
                }
                    if(botShot){
                        movement::makeMove(levelref, botList[j], ' ', ballref);
                        movement::makeMove(levelref, botList[i], ' ', ballref);
                     }

            }
            }

            //  path finding
            for(int i = 0; i < n; i++){
                if(levelref->bombPlanted){
                    int pastV[numVerts];
                    int cpuGPS = (botList[i]->x * levelref->width) + botList[i]->y;
                    int c4GPS = (bomb->x * levelref->width) + bomb->y;
                    bool smartMove = breadSearch(aList, cpuGPS, c4GPS, numVerts, pastV);
                     //path stores the movements
                    vector<int> path;
                    path.push_back(c4GPS);
                    int previousNode = c4GPS;

                    //and then uses that as an index for past vertices
                    while(pastV[previousNode] != -1){
                        path.push_back(pastV[previousNode]);
                        previousNode = pastV[previousNode];
                    }

                    if(smartMove){
                    if (path[path.size() - 2] == cpuGPS + 1){
                        movement::makeMove(levelref, botList[i], 'd', ballref);
                    } else if (path[path.size() - 2] == cpuGPS - 1){
                        movement::makeMove(levelref, botList[i], 'a', ballref);
                    } else if (path[path.size() - 2] == cpuGPS + levelref->width){
                        movement::makeMove(levelref, botList[i], 's', ballref);
                    } else if (path[path.size() - 2] == cpuGPS - levelref->width){
                        movement::makeMove(levelref, botList[i], 'w', ballref);
                    }
                    }else {
                        char botMove = rngMove[rand() % 5];
                        movement::makeMove(levelref, botList[i], botMove, ballref);
                      }
                      // same algorithm here but for bomb planters
                } else if(botList[i]->bomb){
                    if(!targetSite){
                        bombsiteGPS = siteOpts[rand() % numSites];
                        targetSite = true;
                    }
                    int pastV[numVerts];
                    int cpuGPS = (botList[i]->x * levelref->width) + botList[i]->y;
                    bool smartMove = breadSearch(aList, cpuGPS, bombsiteGPS, numVerts, pastV);


                    pathtoSite.push_back(bombsiteGPS);
                    int previousNode = bombsiteGPS;


                    while(pastV[previousNode] != -1){
                        pathtoSite.push_back(pastV[previousNode]);
                        previousNode = pastV[previousNode];
                    }

                    if(smartMove){
                    if (pathtoSite[pathtoSite.size() - 2] == cpuGPS + 1){
                        movement::makeMove(levelref, botList[i], 'd', ballref);
                    } else if (pathtoSite[pathtoSite.size() - 2] == cpuGPS - 1){
                        movement::makeMove(levelref, botList[i], 'a', ballref);
                    } else if (pathtoSite[pathtoSite.size() - 2] == cpuGPS + levelref->width){
                        movement::makeMove(levelref, botList[i], 's', ballref);
                    } else if (pathtoSite[pathtoSite.size() - 2] == cpuGPS - levelref->width){
                        movement::makeMove(levelref, botList[i], 'w', ballref);
                    }
                    }
                }
                else {
                        char botMove = rngMove[rand() % 5];
                        movement::makeMove(levelref, botList[i], botMove, ballref);
                }
            }
        }

};



//---------------------------------------------------------------------
int main(int argc, char **argv){
//	srand(time(NULL)); //Comment out to always have the same RNG for debugging
    charMap *map = (argc == 2) ? new charMap(argv[1]) : NULL; //Read in input file
    if(map == NULL){printf("Invalid Map File\n"); return 1;}  //close if no file given
    initCurses(); // Curses Initialisations
    //map->print();
    mapEngine *level = new mapEngine( map );
    playerType *player1 = (level->userMenu());
    projEngine *proj = new projEngine(level);
    AItype *AI = new AItype(level, proj);
    char ch;
    // GAME LOOP

    while((ch = movement::readkeyInput()) != 'q'){
      movement::makeMove(level, player1, ch, proj);
      AI->updateAll();
      proj->updateAll();
      level->clearScreen();
      level->renderMap();
      if( level->checkRoundStatus() > 0){
        break;
      }
    }
    delete AI; AI = NULL;
    delete proj; proj = NULL;
    delete player1; player1 = NULL;
    delete level; level = NULL;
    delete map; map = NULL;
    printw("\ngg ez\n");
    endCurses(); //END CURSES
    return 0;
}
//---------------------------------------------------------------------
void initCurses(){
    // Curses Initialisations
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	printw("Welcome - Press Q to Exit\n");
}
void endCurses(){
	refresh();
	getch(); //Make user press any key to close
	endwin();
}
//---------------------------------------------------------------------

//charMap Functions
charMap::charMap(char *arg){
    char temp;
    ifstream fin(arg);
    fin >> mapName;
    fin >> height;
    fin >> temp;
    fin >> width;
    map = new char*[height]; //Allocate our 2D array
    for(int i=0; i<height; i++){
        map[i] = new char[width];
        for(int j=0; j<width; j++) //Read into our array while we're at it!
            fin >> (map[i][j]) >> noskipws; //dont skip whitespace
        fin >> skipws; //skip it again just so we skip linefeed
    }
    //for(int i=0; i<height; i++){ //Uncomment if you have issues reading
    //    for(int j=0; j<width; j++) printf("%c", map[i][j]); printf("\n");};
}
//---------------------------------------------------------------------
charMap::~charMap(){
    if(map == NULL) return;
    for(int i=0; i<height; i++)
        delete [] map[i];
    delete [] map;
}
//---------------------------------------------------------------------
void charMap::print(){ //call only after curses is initialized!
    printw("Read Map: '%s' with dimensions %dx%d!\n",
            mapName.c_str(), height, width);
    //Note the c_str is required for C++ Strings to print with printw
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++)
            printw("%c", map[i][j]);
        printw("\n");
    }
}
//---------------------------------------------------------------------
