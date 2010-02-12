package magiclegoblimps;

import java.awt.geom.Point2D.Double;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.util.*;
import java.text.*;

class RobotMapPanel extends JPanel{
    Color backgroundColor = Color.GRAY;
   
    int x_count;
    int y_count;
    int spacing;
    int trim = MLBFrame.TRIM;
    ArrayList<MovableEntity> me = null;
    HashMap<ObjectOfInterest, Point2D.Double> ls;
    HashMap<magiclegoblimps.Robot, ObjectOfInterest> as;
    HashMap<ObjectOfInterest, magiclegoblimps.Robot> vf;
    HashMap<ObjectOfInterest, java.lang.Double> qos;
    DecimalFormat df = new DecimalFormat("0.00");

    public RobotMapPanel(int x, int y, int s){

        setPreferredSize(new Dimension(500,600));
        
        x_count = x;
        y_count = y;
        spacing = s;

    }


    @Override
    public void paintComponent(Graphics g){

    
        //Clear Screen
        g.setColor(backgroundColor);
        ((Graphics2D)g).setStroke(new BasicStroke(4));
        g.fillRect(0,0,getWidth(),getHeight());
        //Draw Grid
        g.setColor(Color.BLACK);
        for(int i = 0; i < x_count; i++){
            for(int j = 0; j < y_count; j++){
                g.drawRect(trim+i*spacing, trim+j*spacing, spacing, spacing);
            }
        }
        if(me!=null){
            for(MovableEntity rr : me){
                if(rr instanceof Robot) {
                    g.setColor(new Color(255,0,0));
                } else if(rr instanceof ObjectOfInterest) {
                    // OOIs set their own color
                } else {
                    g.setColor(Color.BLACK);
                }
                rr.draw(g);
            }
        if(as!=null && vf != null && qos != null){
            int i = 0;
            g.setColor(Color.BLUE);
            g.drawString("Assignments: Name Tracking ViewedFrom QoS",MLBFrame.TRIM, MLBFrame.GRID_Y*MLBFrame.SPACING+MLBFrame.TRIM*2 + i*14);
            i+=2;
            for(ObjectOfInterest ooi : vf.keySet()){
                String s = "";
                for(Robot r : as.keySet()){
                    if(as.get(r).equals(ooi)){
                        s+= r.name + " ";
                    }
                }
                if(s.length() == 0){
                    s = "null";
                }
                String s2 = vf.get(ooi)==null?"null":vf.get(ooi).name;
                g.drawString(ooi.name + ":  " + s +"  "+s2+"  " +  df.format(qos.get(ooi)),MLBFrame.TRIM, MLBFrame.GRID_Y*MLBFrame.SPACING+MLBFrame.TRIM*2 + i*14);
                
                i++;
            }
        }

        }
        g.setColor(Color.WHITE);
        if(ls!=null){
            for(ObjectOfInterest o : ls.keySet()){
                if(ls.get(o)!=null){
                    g.drawString(o.name,(int)(ls.get(o).x-20),(int)(ls.get(o).y-10));
                    g.drawString("X",(int)ls.get(o).x,(int)ls.get(o).y);
                }
            }
        }
    }
}
