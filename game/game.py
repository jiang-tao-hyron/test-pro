# 1 - Import library
# -*- coding: UTF-8 -*-
import math
import random
import pygame
from pygame.locals import *

pygame.init()
width, height = 640, 480
screen=pygame.display.set_mode((width, height))

acc=[0,0]

# Bad guys
class Bad:
    '敌人'
    badguyimg = [pygame.image.load("resources/images/badguy.png"),pygame.image.load("resources/images/badguy2.png"),\
                 pygame.image.load("resources/images/badguy3.png"),pygame.image.load("resources/images/badguy4.png")]
    red = [pygame.image.load("resources/images/red1.png"),pygame.image.load("resources/images/red2.png"),\
                 pygame.image.load("resources/images/red3.png"),pygame.image.load("resources/images/red4.png")]
    blood = pygame.image.load("resources/images/blood.png")
    
    def __init__(self, kind, x, y):
        self.kind = kind
        self.x = x
        self.y = y
        if kind ==1:
            self.img = Bad.red
            self.speed = 3
            self.blood = 10
            self.bloodmax = 10
        if kind ==2:
            self.img = Bad.badguyimg
            self.speed = 2
            self.blood = 30
            self.bloodmax = 30

    def Draw(self):
        self.x -= self.speed    # Bad guy speed
        ph = self.x/40%4
        screen.blit(self.img[ph], (self.x,self.y))
        for loop in range(0,32):
            if self.blood*1.0/self.bloodmax < 1-loop*1.0/32:
                screen.blit(Bad.blood, (self.x+13+loop,self.y+12))
badtimer = 100
badguys=[]

# Player
class Player:
    player = pygame.image.load("resources/images/dude.png")
    powerbar = pygame.image.load("resources/images/powerbar.png")
    power = pygame.image.load("resources/images/power.png")
    def __init__(self, name, x, y, angle, speed, power):
        self.name = name
        self.x = x
        self.y = y
        self.angle = angle
        self.speed = speed
        self.power = power
        self.pwrcnt = 0

    def Draw(self, position):
        screen.blit(Player.powerbar, (220,5))
        for power1 in range(self.power):
            screen.blit(Player.power, (power1+222,8))
        
        self.angle = math.atan2(position[1]-self.y,position[0]-self.x)
        playerrot = pygame.transform.rotate(Player.player, 360-self.angle*57.29)
        playerpos1 = (self.x-playerrot.get_rect().width/2, self.y-playerrot.get_rect().height/2)
        screen.blit(playerrot, playerpos1)
        
        playerrect=pygame.Rect(Player.player.get_rect())
        playerrect.left=self.x
        playerrect.top=self.y
        index=0
        for gold in golds:
            goldrect=pygame.Rect(gold.goldimg.get_rect())
            goldrect.top=gold.y
            goldrect.left=gold.x
            if playerrect.colliderect(goldrect):
                self.power += gold.kind*10
                golds.pop(index)
            index += 1
        self.pwrcnt += 1
        if self.pwrcnt > 30:
            self.power += 1
            self.pwrcnt = 0
        if self.power > 195:self.power = 195
pl1 = Player("tao", 100, 100, 0, 3, 50)
keys = [False, False, False, False]

# arrows
class Arrows:
    arrow = pygame.image.load("resources/images/bullet.png")
    def __init__(self, angle, x, y, speed, power):
        self.x = x
        self.y = y
        self.angle = angle
        self.speed = speed
        self.power = power
    def Draw(self):
        self.x += math.cos(self.angle)*self.speed
        self.y += math.sin(self.angle)*self.speed
        arrow1 = pygame.transform.rotate(Arrows.arrow, 360-self.angle*57.29)
        screen.blit(arrow1, (self.x, self.y))
arrows=[]

# gold
class Gold:
    gold1 = pygame.image.load("resources/images/gold1.png")
    gold2 = pygame.image.load("resources/images/gold2.png")
    
    def __init__(self, x, y, kind):
        self.x = x
        self.y = y
        self.kind = kind
        if self.kind == 1:
            self.goldimg = Gold.gold1
        elif self.kind == 2:
            self.goldimg = Gold.gold2

    def Draw(self):
        screen.blit(self.goldimg, (self.x, self.y))
golds = []
# grand
class Grand:
    grass = pygame.image.load("resources/images/grass.png")
    castle = pygame.image.load("resources/images/castle.png")
    healthbar = pygame.image.load("resources/images/healthbar.png")
    health = pygame.image.load("resources/images/health.png")
    def __init__(self,kind):
        self.kind = kind
        self.healthvalue=194
    
    def Draw(self):
        screen.fill(0)
        for x in range(width/Grand.grass.get_width()+1):
            for y in range(height/Grand.grass.get_height()+1):
                screen.blit(Grand.grass,(x*100,y*100))
        screen.blit(Grand.castle,(0,30))
        screen.blit(Grand.castle,(0,135))
        screen.blit(Grand.castle,(0,240))
        screen.blit(Grand.castle,(0,345 ))

        screen.blit(Grand.healthbar, (5,5))
        for health1 in range(self.healthvalue):
            screen.blit(Grand.health, (health1+8,8))

        font = pygame.font.Font(None, 24)
        survivedtext = font.render("fps:"+ str(fps) +"  "+str((90000-pygame.time.get_ticks())/60000)\
                                   +":"+str((90000-pygame.time.get_ticks())/1000%60).zfill(2), True, (0,0,0))
        textRect = survivedtext.get_rect()
        textRect.topright=[635,5]
        screen.blit(survivedtext, textRect)
bg = Grand(0)

def DrawArrow():
    for arrow in arrows:
        arrow.Draw()

def BuildBad():
    global badtimer
    badtimer-=1
    if badtimer==0:
        badguys.append(Bad(random.randint(1,2),640, random.randint(50,430)))
        badtimer = 100

def Kill():
    index=0
    for badguy in badguys:
        # 6.3.1 - Kill castle
        badrect=pygame.Rect(Bad.badguyimg[0].get_rect())
        badrect.top=badguy.y
        badrect.left=badguy.x
        if badrect.left<64:
            bg.healthvalue -= random.randint(5,20)
            badguys.pop(index)

        index1=0
        for bullet in arrows:
            if bullet.x<-64 or bullet.x>640 or bullet.y<-64 or bullet.y>480:
                arrows.pop(index1)
            bullrect=pygame.Rect(Arrows.arrow.get_rect())
            bullrect.left=bullet.x
            bullrect.top=bullet.y
            if badrect.colliderect(bullrect):
                acc[0]+=1
                arrows.pop(index1)
                if bullet.power >= badguy.blood:
                    badguys.pop(index)
                    golds.append(Gold(badguy.x,badguy.y,badguy.kind))
                else:
                    badguy.blood -= bullet.power
            index1+=1
        index+=1
        
        playerrect=pygame.Rect(Player.player.get_rect())
        playerrect.left=pl1.x
        playerrect.top=pl1.y
        if badrect.colliderect(playerrect):
            if pl1.power > 0:
                pl1.power -= 1

def DrawGold():
    for badguy in badguys:
        badguy.Draw()

def DrawBadGuy():
    for gold in golds:
        gold.Draw()

def Event():
    for event in pygame.event.get():
        # check if the event is the X button 
        if event.type==pygame.QUIT:
            # if it is quit the game
            pygame.quit() 
            exit(0)
        if event.type == pygame.KEYDOWN:
            if event.key==K_w:
                keys[0]=True
            elif event.key==K_a:
                keys[1]=True
            elif event.key==K_s:
                keys[2]=True
            elif event.key==K_d:
                keys[3]=True
        if event.type == pygame.KEYUP:
            if event.key==pygame.K_w:
                keys[0]=False
            elif event.key==pygame.K_a:
                keys[1]=False
            elif event.key==pygame.K_s:
                keys[2]=False
            elif event.key==pygame.K_d:
                keys[3]=False
        if event.type==pygame.MOUSEBUTTONDOWN:
            if pl1.power > 0:
                acc[1]+=1
                arrows.append(Arrows(pl1.angle,pl1.x+35*math.cos(pl1.angle),pl1.y+35*math.sin(pl1.angle), 3, 10))
                pl1.power -=5
    # 9 - Move Player
    if keys[0]:
        pl1.y-=3
    elif keys[2]:
        pl1.y+=3
    if keys[1]:
        pl1.x-=3
    elif keys[3]:
        pl1.x+=3

def Go():
    bg.Draw()
    DrawGold()
    pl1.Draw(pygame.mouse.get_pos())
    DrawArrow()
    BuildBad()
    Kill()
    DrawBadGuy()
    #draw
    pygame.display.flip()

oldtime = 0
win = 0
while 1:
    newtime = pygame.time.get_ticks()
    if newtime - oldtime >= 20:
        fps = 1000 / (newtime - oldtime)
        oldtime = newtime
        Event()
        Go()
    if bg.healthvalue < 0:
        win = 0
        break
    if newtime > 90000:
        win = 1
        break
if win:
    screen.blit(pygame.image.load("resources/images/youwin.png"), (0, 0))
else:
    screen.blit(pygame.image.load("resources/images/gameover.png"), (0, 0))
pygame.display.flip()
while 1:
    Event()