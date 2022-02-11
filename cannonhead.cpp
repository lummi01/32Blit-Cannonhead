// Cannonhead Clash
// 2022 M. Gerloff


#include "cannonhead.hpp"
#include "assets.hpp"

#define LEVEL 16
 
using namespace blit;

Font font(font8x14);

struct Player 
{
    bool xflip;
    float ani;
    int x;
    float y;
    float dy;
    bool is_shot;
    float shot_power;
    float shot_x;
    float shot_y;
    float shot_dx;
    int score;
};

struct Explosion
{
    int alpha;
    float x;
    float y;
    float dx;
    float dy;
};

struct Wave
{
    int timer;
    int ani;
};

Player p[2];
Explosion explosion[8];
Explosion rocket;
Wave wave;

int menu_setting[5]{0,1,1,16,0};
int menu_item = 4;

int state = 0;

int wall[40][14];
int level;

int cpu_status = 3;
int cpu_status2 = 3;

int counter;

void new_explosion(float x, float y)
{
    for (int t=0; t<4; t++)
    {
        for (int i=0; i<8; i++)
        {
            if (explosion[i].alpha == 0)
            {
                explosion[i].alpha = 255 - rand() % 128;
                explosion[i].x = x;
                explosion[i].y = y;
                explosion[i].dx = ((rand() % 20) - 10) * .1f;
                explosion[i].dy = ((rand() % 15) * -.1f);
                break;
            }
        }
    }
}

void update_explosion()
{
    for (int i=0; i<8; i++)
    {
        if (explosion[i].alpha > 0)
        {
            explosion[i].dy+=.1f;
            explosion[i].x+=explosion[i].dx;
            explosion[i].y+=explosion[i].dy;
            explosion[i].alpha-=5;
            if (explosion[i].alpha < 0)
            {
                explosion[i].alpha = 0;
            }
        }
    }
}

void update_wave()
{
    wave.timer++;
    if (wave.timer>15)
    {
        wave.timer = 0;
        wave.ani+=8;
        if (wave.ani>56)
        {
            wave.ani = 0;
        }
    }
}

void render_player()
{
    screen.pen = Pen(255, 255, 255);
    for (int i=0;i<2;i++)
    {
        screen.sprite((i*16) + p[i].ani, Point(p[i].x, p[i].y), p[i].xflip);
        if (p[i].is_shot)
        {
            screen.rectangle(Rect(p[i].shot_x,p[i].shot_y,2,2));
        }   
    }
}

void render_explosion()
{
    for (int i=0; i<8; i++)
    {
        if (explosion[i].alpha > 0)
        {
            screen.alpha = explosion[i].alpha;
            screen.pen = Pen(255,255,255);
            screen.rectangle(Rect(explosion[i].x,explosion[i].y,2,2));
        }
    }
}

void start()
{
    p[0].ani = 0;
    p[0].x = 20;
    p[0].y = 60;
    p[0].xflip = false;
    p[0].is_shot = false;
    p[0].shot_power = 0;

    p[1].ani = 0;
    p[1].x = 132;
    p[1].y = 60;
    p[1].xflip = true;
    p[1].is_shot = false;
    p[1].shot_power = 0;

    level = menu_setting[3];
    if (menu_setting[3]>=LEVEL)
    {
        menu_setting[3]++;
        if (menu_setting[3]>=LEVEL *2)
        {
            menu_setting[3]=LEVEL;
        }
        level = menu_setting[3] -LEVEL;
    }    

    int scrn[LEVEL][20]{
        {0,0,1,4099,8198,4102,8204,4108,28,24,24,24,1052,2060,1036,2054,1030,3,1,0},
        {0,0,1,1,3,2,6,4,12,8,24,16,48,32,96,64,192,128,128,0},
        {0,0,1,1,17,17,273,273,4369,4369,4369,4369,4369,273,273,17,17,1,1,0},
        {0,0,256,384,192,96,48,24,12,6,3,6,12,24,48,96,192,384,256,0},
        {0,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,16383,16383,0},
        {0,0,0,1008,1032,2052,4098,8193,8193,8193,8193,8193,8193,4098,2052,1032,1008,0,0,0},
        {0,0,7936,4352,4352,4352,4352,8176,272,272,272,272,511,17,17,17,17,31,0,0},
        {0,0,8195,8195,0,4102,4102,0,2060,2060,0,1048,1048,0,560,560,0,352,352,0},
        {0,0,8456,4228,2114,1057,528,264,132,66,33,16,8,4,2,1,0,0,0,0},
        {0,0,0,0,15,0,15,0,15,0,0,0,0,0,0,0,0,0,0,16384},
        {0,0,0,64,32,64,32,64,32,64,32,64,32,64,32,64,32,64,0,0},
        {0,0,8736,8736,8736,8736,8736,8736,544,544,544,544,32,32,32,32,3,3,3,3},
        {0,0,3072,1536,768,384,192,96,48,96,192,384,768,384,192,96,48,24,12,0},
        {0,0,24,30,15,7,7,3,3,2047,523,275,227,7,6,12,8,8,0,0},
        {0,0,0,3,3,3,3,3,3,255,255,192,192,192,192,16320,16320,12288,12288,12288},
        {0,0,0,6144,15360,15360,6240,240,240,6241,15363,15363,6241,240,240,6241,15363,15363,6241,240}};

    for (int s = 0; s < 20; s++)
    {
        int bck = scrn[level][s];
        int bny = 8192;
        for (int z=0; z<14; z++)
        {
            if (bck >= bny)
            {
                wall[s][z] = (rand() %3 *16) +128;
                wall[39-s][z] = (rand() %3 *16) +128;
                bck-= bny;
            }
            else
            {
                wall[s][z] = 0;
                wall[39-s][z] = 0;
            }
            bny*=.5;
        }
    }
}

void player_ani(int player)
{
    p[player].ani+=.2f;
    if (p[player].ani>=2)
    {
        p[player].ani = 0;
    }
}

bool is_ground(int x, int y)
{
    if (x<0 || x>159 || y<48 || y>103)
    {
        return false;
    }
    else
    {
        int rx = x * .25f;
        int ry = (y - 48) * .25f;

        if (wall[rx][ry]>0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool is_player(int player, int x, int y)
{
    if (x<p[player].x +8 && x>p[player].x && y<p[player].y +8 && y>p[player].y)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void del_ground(int x, int y)
{
    int rx = x * .25f;
    int ry = (y - 48) * .25f;
    new_explosion(x, y);
    wall[rx][ry] = 0;
}

void player_move(int p1, int p2, int status)
{
    if (p[p1].y>98)
    {
        p[p1].y+=.2f;
        if (p[p1].y > 120 && !p[p1].is_shot && !p[p2].is_shot)
        {
            if (p[p2].score>9)
            {
                state=p2 +2;
            }
            else
            {
                start();
            }
        }        
    }
    else if (p[p1].ani == 3)
    {
        p[p1].dy-=.1f;
        p[p1].y-=p[p1].dy;
    }
    else if (!is_ground(p[p1].x +1, p[p1].y +8) && !is_ground(p[p1].x +4, p[p1].y +8) && !is_ground(p[p1].x +6, p[p1].y +8)) // free fall
    {
        p[p1].y+=1;
        if (p[p1].y>98)
        {
            p[p2].score++;
        }
    }
    else
    {
        switch(status)
        {
            case 1: // power up shot
                if (!p[p1].is_shot)
                {
                    p[p1].ani = 2;
                    p[p1].shot_power+=.03f;
                    if (p[p1].shot_power>3.5f)
                    {
                        p[p1].shot_power = 3.5f;
                    }
                }
                break;
            case 2: // shot
                if (!p[p1].is_shot)
                {
                    p[p1].shot_x = p[p1].x +3;
                    p[p1].shot_y = p[p1].y;
                    p[p1].is_shot = true;
                    p[p1].shot_dx = 1.2f;
                    if (p[p1].xflip)
                    {
                        p[p1].shot_dx = -p[p1].shot_dx;
                    }
                }
                break;
            case 3: // move left
                p[p1].xflip = true;
                p[p1].x--;
                player_ani(p1);
                if (is_ground(p[p1].x +1, p[p1].y +4) || is_ground(p[p1].x +1, p[p1].y) || p[p1].x<0)
                {
                    p[p1].x++;
                    if (is_ground(p[p1].x, p[p1].y +4) && !is_ground(p[p1].x -1, p[p1].y) && !is_ground(p[p1].x -1, p[p1].y -4))
                    {
                        p[p1].x--;
                        p[p1].y-=4;
                    }
                }
                break;
            case 4: // move right
                p[p1].xflip = false;
                p[p1].x++;
                player_ani(p1);
                if (is_ground(p[p1].x +6, p[p1].y +4) || is_ground(p[p1].x +6, p[p1].y) || p[p1].x>151)
                {
                    p[p1].x--;
                    if (is_ground(p[p1].x +7, p[p1].y +4) && !is_ground(p[p1].x +8, p[p1].y) && !is_ground(p[p1].x +8, p[p1].y -4))
                    {
                        p[p1].x++;
                        p[p1].y-=4;
                    }
                }            
                break;
            case 5: // stay
                p[p1].ani = 0;
                break;
        }
    }
}

void player_control()
{
    int status = 5;
    if (buttons & Button::DPAD_UP || (menu_setting[1]==1 && buttons & Button::A))
    {
        status = 1;
    }
    else if (buttons.released & Button::DPAD_UP || (menu_setting[1]==1 && buttons.released & Button::A))
    {
        status = 2;
    }
    else if (buttons & Button::DPAD_LEFT)
    {
        status = 3;
    }
    else if (buttons & Button::DPAD_RIGHT)
    {
        status = 4;
    }
    player_move(0,1,status);
}

void player2_control()
{
    int status = 5;
    
    if (buttons & Button::X || (menu_setting[0]==1 && buttons & Button::DPAD_UP))
    {
        status = 1;
    }
    else if (buttons.released & Button::X || (menu_setting[0]==1 && buttons.released & Button::DPAD_UP))
    {
        status = 2;
    }
    else if (buttons & Button::Y)
    {
        status = 3;
    }
    else if (buttons & Button::A)
    {
        status = 4;
    }
    player_move(1,0,status);
}

void cpu_control()
{
    switch(cpu_status)
    {
        case 1:
        {
            if (p[1].shot_power >= 3.5f || p[1].shot_power > (p[1].x - p[0].x) * ((22 + rand() %7) *.001f))
            {
                cpu_status = 2;
            }
            break;
        }
        case 2:
        {
            if (!p[1].is_shot)
            {
                cpu_status = rand() %1 + 3;
            }
            break;
        }
        case 3:
        {
            if (is_ground(p[1].x -1, p[1].y) || (!is_ground(p[1].x -1, p[1].y +8) && !is_ground(p[1].x +3, p[1].y +8) && !is_ground(p[1].x -1, p[1].y +12) && !is_ground(p[1].x +3, p[1].y +12)))
            {
                cpu_status = 4;
            }
            else if (p[1].x>p[0].x -8 && rand() %50<1)
            {
                cpu_status = 1;
            }
            break;
        }
        case 4:
        {
            if (is_ground(p[1].x +8, p[1].y) || (!is_ground(p[1].x +4, p[1].y +8) && !is_ground(p[1].x +8, p[1].y +8) && !is_ground(p[1].x +4, p[1].y +12) && !is_ground(p[1].x +8, p[1].y +12)))
            {
                cpu_status = 3;
            }
            else if (p[1].x<p[0].x +8 && rand() %50<1)
            {
                cpu_status = 1;
            }
            break;
        }
    }
    player_move(1,0,cpu_status);
}

void cpu_control2()
{
    switch(cpu_status2)
    {
        case 1:
        {
            if (p[0].shot_power >= 3.5f || p[0].shot_power > (p[1].x - p[0].x) * .025f) //((22 + rand() %7) *.001f))
            {
                cpu_status2 = 2;
            }
            break;
        }
        case 2:
        {
            if (!p[0].is_shot)
            {
                cpu_status2 = rand() %1 + 3;
            }
            break;
        }
        case 3:
        {
            if (is_ground(p[0].x -1, p[0].y) || (!is_ground(p[0].x -1, p[1].y +8) && !is_ground(p[0].x +3, p[0].y +8) && !is_ground(p[0].x -1, p[0].y +12) && !is_ground(p[0].x +3, p[0].y +12)))
            {
                cpu_status2 = 4;
            }
            else if (p[0].x>p[1].x -8 && rand() %50<1)
            {
                cpu_status2 = 1;
            }
            break;
        }
        case 4:
        {
            if (is_ground(p[0].x +8, p[0].y) || (!is_ground(p[0].x +4, p[0].y +8) && !is_ground(p[0].x +8, p[1].y +8) && !is_ground(p[0].x +4, p[0].y +12) && !is_ground(p[0].x +8, p[0].y +12)))
            {
                cpu_status2 = 3;
            }
            else if (p[0].x<p[1].x +8 && rand() %50<1)
            {
                cpu_status2 = 1;
            }
            break;
        }
    }
    player_move(0,1,cpu_status2);
}

void update_shot(int p1, int p2)
{
    if (p[p1].is_shot)
    {
        p[p1].shot_x+=p[p1].shot_dx;
        p[p1].shot_power-=.06f;
        p[p1].shot_y-=p[p1].shot_power;
        if (is_ground(p[p1].shot_x, p[p1].shot_y))
        {
            del_ground(p[p1].shot_x, p[p1].shot_y);
            p[p1].is_shot = false;
            p[p1].shot_power = 0;
        }
        if (p[p1].shot_y>104)
        {
            p[p1].is_shot = false;
            p[p1].shot_power = 0;
        }
        if (is_player(p2, p[p1].shot_x, p[p1].shot_y))
        {
            p[p1].is_shot = false;
            p[p1].shot_power = 0;
            p[p1].score++;
            p[p2].ani = 3;
            p[p2].dy = 2;
        }
    }
}
void update_menu()
{
    int menu_max[5]{1,1,2,16,0};

    if (buttons.released & Button::DPAD_UP && menu_item>0)
    {
        menu_item--;
    }
    else if (buttons.released & Button::DPAD_DOWN && menu_item<4)
    {
        menu_item++;
    }
    else if (buttons.released & Button::DPAD_LEFT)
    {
        menu_setting[menu_item]--;
        if (menu_setting[menu_item]<0)
        {
            menu_setting[menu_item]=menu_max[menu_item];
        }
    }
    else if (buttons.released & Button::DPAD_RIGHT)
    {
        menu_setting[menu_item]++;
        if (menu_setting[menu_item]>menu_max[menu_item])
        {
            menu_setting[menu_item]=0;
        }
    }
    else if (buttons.released & Button::A && menu_item==4)
    {
        if (menu_setting[3]>=LEVEL)
        {
            menu_setting[3] = LEVEL + (rand()%LEVEL);
        }
        p[0].score = 0;
        p[1].score = 0;
        state = 1;
        start();
    }
}

void render_menu()
{
    std::string menu[3]={"cpu skill","level","    start"};
    std::string select[5][17]={
        {"human","cpu"},
        {"human","cpu"},
        {"smart","normal","stupid"},
        {"islands","up & down","trees","double 'v'","walls","circles","boxes","big 'x'","bevels","pong-styl","basic","shelves","graph","pirates","pyramid","bubbles","mixed"},
        {"start   "}};
    int select_y[5]={38,50,69,90,104};

    screen.pen = Pen(0,0,0);
    screen.clear(); 
    screen.pen = Pen(255, 255, 255);
    screen.sprite(Rect(0,4,7,3),Point(55,8));  
    screen.sprite(Point(0,0),Point(55,38));
    screen.sprite(Point(0,1),Point(55,50));
    for (int i=0;i<3;i++)
    {
        screen.pen = Pen(150,100,50);
        screen.text(menu[i], minimal_font, Point(55,63+(i*21)), true, TextAlign::top_left); 
        screen.pen = Pen(223,113,38);
        screen.text(menu[i], minimal_font, Point(55,62+(i*21)), true, TextAlign::top_left);  
    }
    for (int i=0;i<5;i++)
    {
        std::string txt = select[i][menu_setting[i]];
        if (menu_setting[i]>=LEVEL)
        {
            txt = "mixed";
        }
        if (menu_item == i)
        {
            screen.pen = Pen(200,200,200);
            screen.text(txt, minimal_font, Point(105,select_y[i]+1), true, TextAlign::top_right); 
            screen.pen = Pen(255,255,255);
            screen.text(txt, minimal_font, Point(105,select_y[i]), true, TextAlign::top_right); 
        }
        else if (i<4)
        {
            screen.pen = Pen(100,100,100);
            screen.text(txt, minimal_font, Point(105,select_y[i]+1), true, TextAlign::top_right); 
            screen.pen = Pen(150,150,150);
            screen.text(txt, minimal_font, Point(105,select_y[i]), true, TextAlign::top_right); 
        }        
    }
}

void update_winner(int p1)
{
    p[p1].dy+=.06f;
    p[p1].y+=p[p1].dy;
    if (p[p1].y>=50)
    {
        p[p1].y=50;
        p[p1].dy=-1;
        p[p1].xflip=!p[p1].xflip;
        counter++;
    }
    rocket.dy+=.05f;
    if (rocket.dy>=0)
    {
        new_explosion(rocket.x,rocket.y);
        rocket.x = 70 + rand() %20;
        rocket.dx = (rand() %20 -10) *.1f;
        rocket.y = 120;
        rocket.dy = -(rand() %15 +20) * .1f;
    }
    rocket.x+=rocket.dx;
    rocket.y+=rocket.dy;
    update_explosion();
    if (counter>=25)
    {
        counter = 0;
        state = 0;
    }
}

void render_winner(int p1)
{
    screen.pen = Pen(0,0,0);
    screen.clear(); 
    screen.pen = Pen(255, 255, 255);
    screen.rectangle(Rect(rocket.x,rocket.y,2,2));  
    render_explosion();
    screen.alpha = 255;
    screen.stretch_blit(screen.sprites,Rect(0, p1 * 8, 8, 8), Rect(72, p[p1].y, 16, 16),p[p1].xflip);        
    p1 == 0 ? screen.pen = Pen(150,0,0): screen.pen = Pen(0,150,0);
    screen.text("WINNER", font, Point(80, 75), true, TextAlign::center_center); 
    p1 == 0 ? screen.pen = Pen(255,0,0): screen.pen = Pen(0,255,0);
    screen.text("WINNER", font, Point(80, 74), true, TextAlign::center_center); 
}


//
// init()
//

void init() 
{
    set_screen_mode(ScreenMode::lores);
    screen.sprites = Surface::load(asset_sprites);

    channels[0].waveforms = Waveform::NOISE; 
    channels[0].volume = 0x0fff;
    channels[0].frequency = 1000;
    channels[0].attack_ms = 5;
    channels[0].decay_ms = 50;
    channels[0].sustain = 0;
    channels[0].release_ms = 5;
}

//
// render(time)
//

void render(uint32_t time) 
{
    screen.pen = Pen(255,255,255);
    screen.alpha = 255;
    screen.mask = nullptr;

    if (state == 0) 
    {
        render_menu();
    }
    else if (state == 1) 
    {
        for (int i=0; i<120; i+=8)
        {
            screen.pen = Pen(255-i,32,255);
            screen.rectangle(Rect(0,i,160,8));
        }

        screen.pen = Pen(255, 0, 0);
        screen.text(std::to_string(p[0].score), font, Point(8, 10)); 
        if (p[0].shot_power > 0)
        {
            screen.rectangle(Rect(8,4,p[0].shot_power *15,3));
        }

        screen.pen = Pen(0, 255, 0);
        screen.text(std::to_string(p[1].score), font, Point(153, 10), true, TextAlign::top_right); 
        if (p[1].shot_power > 0)
        {
            screen.rectangle(Rect(153 -(p[1].shot_power*15),4,p[1].shot_power *15,3));
        }

        for (int z=0; z<14; z++)
        {
            for (int s=0; s<39; s++)
            {
                if (wall[s][z] > 0)
                {
                    screen.pen = Pen(32, wall[s][z], 32);
                    screen.rectangle(Rect(s *4,(z *4) +48,4,4));
                }
            }
        }

        render_player();
        
        render_explosion();

        screen.alpha = 128;
        screen.pen = Pen(0, 50, 255);
        screen.rectangle(Rect(0,104,160,16));
        for (int i = 0; i < 160; i+=40)
        {
            screen.stretch_blit(screen.sprites,Rect(wave.ani, 19, 8, 3), Rect(i, 101, 40, 3)); 
        }     
    }
    else if (state >1)
    {
        render_winner(state -2);
    }
}

//
// update(time)
//

void update(uint32_t time) 
{
    if (state==0)
    {
        update_menu();
    }
    else if (state==1)
    {
        menu_setting[0]==0? player_control(): cpu_control2();
        menu_setting[1]==0? player2_control(): cpu_control();
        if (buttons & blit::Button::MENU)
        {
            state=0;
        }
        update_shot(0, 1);
        update_shot(1, 0);
        update_explosion();
        update_wave();
    }
    else if (state>1)
    {
        update_winner(state -2);
    }
}

