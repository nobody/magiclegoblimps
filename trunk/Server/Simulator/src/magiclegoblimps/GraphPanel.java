/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package magiclegoblimps;
import javax.swing.*;
import java.util.*;
import java.awt.*;
/**
 *
 * @author Kevin Hulin
 */
class GraphPanel extends JPanel{
    public static final int HIST_LENGTH = 500;
    public static final int TRIM = 20;
    public static final int SCALE_TO = 100;
    HashMap<ObjectOfInterest,double[]> history = new HashMap<ObjectOfInterest,double[]>();
    HashMap<ObjectOfInterest,Double> qos = null;
    public GraphPanel(){
        setPreferredSize(new Dimension(HIST_LENGTH,SCALE_TO+2*TRIM));
        
    }
    public void update(){
        if(qos==null){
            return;
        }
        for(ObjectOfInterest ooi : qos.keySet()){
            double[] hist;
            if(history.containsKey(ooi)){
                hist = shiftArray(history.get(ooi),1);
                history.put(ooi,hist);
            }else{

                hist = new double[HIST_LENGTH];
                Arrays.fill(hist,0);
                history.put(ooi,hist);
            }
            hist[hist.length-1] = qos.get(ooi).doubleValue()*(double)SCALE_TO;
           
        }
        
    }

    @Override
    public void paintComponent(Graphics g){
        g.setColor(Color.GRAY);
        g.fillRect(0,0,getWidth(),getHeight());
        g.setColor(Color.BLACK);
        g.drawLine(TRIM,TRIM,TRIM,120);
        g.drawLine(TRIM,SCALE_TO+TRIM,TRIM+HIST_LENGTH,SCALE_TO+TRIM);
        g.drawLine(TRIM,SCALE_TO/2+TRIM,TRIM+HIST_LENGTH,SCALE_TO/2+TRIM);
        for(int i = 0; i <HIST_LENGTH; i++){
            for(ObjectOfInterest ooi : history.keySet()){
             //  System.out.println(history.get(ooi)[i]);
                g.setColor(ooi.color);
                g.fillOval(TRIM+i,(int)(TRIM+SCALE_TO-(history.get(ooi)[i])),2,2);
            }
        }
    }

    public double[] shiftArray(double[] d, int offset){
        double[] ret = new double[d.length];
        for(int i = 0; i < ret.length-offset; i++){
            ret[i] = d[i+offset];
        }
        return ret;
    }


}
