package magiclegoblimps;

import java.awt.geom.Point2D.Double;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.util.*;

class Robot extends MovableEntity{

    public static final int ROBOT_RADIUS = 6; //in pixles for drawing
    public static final double ROBOT_ACCEL = 1; //pixles per tick^2 Not really used...
    public static final double ROBOT_SPEED = 1; //pixles per tick
    public static final double FRICTION = -.3; //stopping accel... again not really used
    public static final Point2D.Double START_POS = new Point2D.Double(20,20); //Robot starting position
    public static final int GRID_WIDTH = MLBFrame.SPACING;//grid width in pixles
    public static final int TRIM = MLBFrame.TRIM;//border trim in pixles
    public static final double CAMERA_ANGLE = Math.PI/4;//viewing angle of camera
    public static final double CAMERA_DIST = 300;//how far the camera can see
    public static final double SWIVEL_SPEED = .02;//how fast the camera rotates

    
    public double accel = 0;
    public double cameraDir = 0;
    public double cameraDirTheta = 0;
    
    public boolean left = false;
    public boolean right = false;
    
    public ObjectOfInterest trackingObject;

    private boolean[] currMoves = null;
    private Polygon viewingArea = null;

    public Robot(String s){
        name = s;
        pos = (Point2D.Double)START_POS.clone();
    }
    
    public void swivelLeft(){
        cameraDirTheta = -SWIVEL_SPEED;
    }
    
    public void swivelRight(){
        cameraDirTheta = SWIVEL_SPEED;
    }
    
    public void stopSwivel(){
        cameraDirTheta = 0;
    }
    
    public void nextLeft(){
        left = true;
        right = false;
    }
    
    public void nextRight(){
        right = true;
        left = false;
    }
    
    public void turnLeft(){
        dir += Math.PI/2;
    }
    
    public void turnRight(){
        dir -= Math.PI/2;
    }
    
    public void go(){
        accel = ROBOT_ACCEL;
    }
    
    public void stop(){
        accel = FRICTION;
    }
    public boolean isMovingEast(){
        return Math.abs(Math.sin(dir))<.00001&&Math.abs(Math.cos(dir)-1)<.00001;
    }
    public boolean isMovingWest(){
        return Math.abs(Math.sin(dir))<.00001 && Math.abs(Math.cos(dir)+1)<.00001;
    }
    public boolean isMovingNorth(){
        return Math.abs(Math.sin(dir)-1)<.00001 && Math.abs(Math.cos(dir))<.00001;
    }
    public boolean isMovingSouth(){
        return Math.abs(Math.sin(dir)+1)<.00001 && Math.abs(Math.cos(dir))<.00001;
    }
    public void goTo(Point2D.Double p){
        if(p==null){// "Search" algorithm: Go to the center of the map and look around
            p = new Point2D.Double(MLBFrame.GRID_X*MLBFrame.SPACING/2+MLBFrame.TRIM, MLBFrame.GRID_Y*MLBFrame.SPACING/2+MLBFrame.TRIM);
            if(cameraDirTheta==0){
                swivelRight();
            }
        }
       // System.out.println(p.toString());
        if(p.distance(pos)<ObjectOfInterest.OPTIMAL_VIEWING_DISTANCE){
            stop();
        }
        else if(isMovingEast()){//Moving East
           // System.out.println("Moving East"+ "(" + pos.x + ","+pos.y +")");

            if(p.x < pos.x){//if object is in behind us

                if(p.y < pos.y){
                    go();
                    nextLeft();
                }else{
                    go();
                    nextRight();
                }
            }else{
                go();
            }
        }
        else if (isMovingNorth()){//Moving North
           // System.out.println("Moving North"+ "(" + pos.x + ","+pos.y +")");

            if(p.y > pos.y){

                
                if(p.x < pos.x){
                    go();
                    nextLeft();
                }else{
                    go();
                    nextRight();
                }
            }else{
                go();
            }
        }
        else if(isMovingWest()){//Moving West
            //System.out.println("Moving West"+ "(" + pos.x + ","+pos.y +")");
            if(p.x > pos.x){

                if(p.y < pos.y){
                    go();
                    nextRight();
                }else{
                    go();
                    nextLeft();
                }
            }else{
                go();
            }
        }
        else if(isMovingSouth()){//Moving South
           // System.out.println("Moving South"+ "(" + pos.x + ","+pos.y +")");
            if(p.y < pos.y){

                if(p.x < pos.x){
                    go();
                    nextRight();
                }else{
                    go();
                    nextLeft();
                }
            }else{
                go();
            }



        }else{
                System.out.println("FAIL");
            }
    }
    
    public boolean[] validMoves(){
        double delta = .015;
        boolean[] ret = new boolean[]{false,false,false};//Can Move forward, left, right

        if(isMovingEast()){//Moving East
           // System.out.println("Moving East"+ "(" + pos.x + ","+pos.y +")");
            //Can continue in +X Direction?
            if( pos.x < TRIM + GRID_WIDTH * MLBFrame.GRID_X)
                ret[0] = true;
            //Can turn?
            if(!ret[0] ||fpart((pos.x-TRIM)/GRID_WIDTH)<delta){//Is at an intersection?
                //Turn left?
                if(pos.y > TRIM){
                    ret[1] = true;
                }
                //Turn right?
                if(pos.y <  TRIM + GRID_WIDTH * MLBFrame.GRID_Y){
                    ret[2] = true;
                }
            }
        }
        else if (isMovingNorth()){//Moving North
           // System.out.println("Moving North"+ "(" + pos.x + ","+pos.y +")");
            //Can continue in -Y Direction?
            if( pos.y > TRIM )
                ret[0] = true;
            //Can turn?
            if(!ret[0] ||fpart((pos.y-TRIM)/GRID_WIDTH)<delta){//Is at an intersection?
                //Turn left?
                if(pos.x > TRIM){
                    ret[1] = true;
                }
                //Turn right?
                if(pos.x <  GRID_WIDTH * MLBFrame.GRID_X){
                    ret[2] = true;
                }
            }
        }
        else if(isMovingWest()){//Moving West
           // System.out.println("Moving West"+ "(" + pos.x + ","+pos.y +")");
            //Can continue in -X Direction?
            if( pos.x > TRIM )
                ret[0] = true;
            //Can turn?
            if(!ret[0] || fpart((pos.x-TRIM)/GRID_WIDTH)<delta){//Is at an intersection?
                //Turn right?
                if(pos.y > TRIM){
                    ret[2] = true;
                }
                //Turn left?
                if(pos.y <  TRIM + GRID_WIDTH * MLBFrame.GRID_Y){
                    ret[1] = true;
                }
            }
        }
        else if(isMovingSouth()){//Moving South
          //  System.out.println("Moving South"+ "(" + pos.x + ","+pos.y +")");
            //Can continue in +Y Direction?
           
            if( pos.y < TRIM + GRID_WIDTH * MLBFrame.GRID_Y )
                ret[0] = true;
            //Can turn?
            if(!ret[0] ||fpart((pos.y-TRIM)/GRID_WIDTH)<delta){//Is at an intersection?
                //Turn right?
                if(pos.x > TRIM){
                    ret[2] = true;
                }
                //Turn left?
                if(pos.x <  GRID_WIDTH * MLBFrame.GRID_X){
                    ret[1] = true;
                }
            }
            
        
       
        }else{//Robot is not moving on the X or Y axis
                System.out.println("FAIL");
            }



        return ret;
    }
    
    public double fpart(double d){
        return d - Math.floor(d);
    }
    
    public void move(){
        currMoves = validMoves();
        //System.out.println(dir);
        if(currMoves[0]){
            pos.x = pos.x + speed*Math.cos(dir);
            pos.y = pos.y - speed*Math.sin(dir);  
        }

        if(left && currMoves[1]){
            turnLeft();
            left = false;
        }else if(right && currMoves[2]){
            turnRight();
            right = false;
        }

        speed = accel<0 ? Math.max(0,speed+accel) : Math.min(ROBOT_SPEED,speed+accel);
        cameraDir += cameraDirTheta;

        viewingArea = new Polygon(new int[]{(int)pos.x,(int)(pos.x+CAMERA_DIST*Math.cos(dir+cameraDir-CAMERA_ANGLE/2)),(int)(pos.x+CAMERA_DIST*Math.cos(dir+cameraDir+CAMERA_ANGLE/2))},
                new int[]{(int)pos.y,(int)(pos.y-CAMERA_DIST*Math.sin(dir+cameraDir-CAMERA_ANGLE/2)),(int)(pos.y-CAMERA_DIST*Math.sin(dir+cameraDir+CAMERA_ANGLE/2))},3);

    }
    
    public boolean canSee(Point2D p){
        if(p==null) return false;
        return viewingArea.contains(p);

    }
    
    public void trackObject(ObjectOfInterest ooi) { //This is done by sending a coordinate to the goTo(point) function.
        // TODO
    }
    
    public boolean centerOnObject(Point2D.Double p, MovableEntity me){//Returns false if object is not in view
        if(p==null){
            return false;
        }
        //Geometry is win.
        double x1 = pos.x;
        double y1 = pos.y;
        double x2 = pos.x + Math.cos(dir+cameraDir);
        double y2 = pos.y - Math.sin(dir+cameraDir);
        double x3 = p.x;
        double y3 = p.y;

        if(y3*(x2-x1)-x3*(y2-y1) < y1*(x2-x1)-x1*(y2-y1)){
            swivelRight();
        }else{
            swivelLeft();
        }

        return canSee(p);
       
    }
    
    void draw(Graphics g){
        g.drawString(name,(int)(pos.x-ROBOT_RADIUS-3*(name.length()/2)),(int)(pos.y-ROBOT_RADIUS-3));
        Color tmp = g.getColor();
        if(viewingArea!=null){
            g.setColor(Color.GREEN);
            ((Graphics2D)g).setStroke(new BasicStroke(1));

            g.drawPolygon(viewingArea);
        }
        g.setColor(tmp);
        g.fillOval((int)(pos.x-ROBOT_RADIUS),(int)(pos.y-ROBOT_RADIUS),(int)(2*ROBOT_RADIUS), (int)(2*ROBOT_RADIUS));
    }
}
