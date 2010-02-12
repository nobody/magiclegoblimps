package magiclegoblimps;

import java.awt.geom.Point2D.Double;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.util.*;

class ObjectOfInterest extends MovableEntity{
    public static final Point2D.Double DEFAULT_POI_POS = new Point2D.Double(100,100);
    public static final int POI_RAIDUS = 4;
    public static final int OPTIMAL_VIEWING_DISTANCE = 70;
    public static final Color DEFAULT_COLOR = new Color(0, 150, 255);
    
    private static Random rand = new Random();

    double speed;

    public Color color;
    
    public ObjectOfInterest(String s, Color c){
        super();
        name = s;
        color = c;
        speed = 1.5;
        pos = (Point2D.Double)DEFAULT_POI_POS.clone();
        dir = 0;
    }
    
    public ObjectOfInterest(String s){
        this(s, DEFAULT_COLOR);
    }
    
    public void move(){
        //System.out.println(name + " " + (int)(dir/(Math.PI/2)));
        double d = rand.nextDouble();
        if(d>.5){
            dir += .1;
        }else{
            dir += -.1;
        }
        
        if(pos.x < MLBFrame.TRIM){
            dir -= Math.PI;
            pos.x = MLBFrame.TRIM;
        }else if(pos.x > MLBFrame.TRIM + MLBFrame.SPACING * MLBFrame.GRID_X){
            dir -= Math.PI;
            pos.x = MLBFrame.TRIM + MLBFrame.SPACING * MLBFrame.GRID_X;
        }else{
            pos.x = pos.x + speed*Math.cos(dir);
        }
        
        if(pos.y < MLBFrame.TRIM){
            dir -= Math.PI;
            pos.y = MLBFrame.TRIM;
        }else if(pos.y > MLBFrame.TRIM + MLBFrame.SPACING * MLBFrame.GRID_Y){
            dir -= Math.PI;
            pos.y = MLBFrame.TRIM + MLBFrame.SPACING * MLBFrame.GRID_Y;
        }else{
            pos.y = pos.y + speed*Math.sin(dir);
        }
    }
    
    void draw(Graphics g){
        g.setColor(color);
        g.drawString(name,
            (int)(pos.x-POI_RAIDUS-3*(name.length()/2)),
            (int)(pos.y-POI_RAIDUS-3));
        g.fillRect((int)(pos.x-POI_RAIDUS),
            (int)(pos.y-POI_RAIDUS),(int)(2*POI_RAIDUS),
            (int)(2*POI_RAIDUS));
    }
}
