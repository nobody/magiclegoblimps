/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package magiclegoblimps;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
/**
 *
 * @author Kevin Hulin
 */
public class DemandPanel extends JPanel {
    MLBController con;
    ArrayList<DemandUnit> du = null;


    public DemandPanel(MLBController mlbc){
        con = mlbc;
        setLayout(new GridLayout(10,1));
        update();
    }

    public void paintComponent(Graphics g){

    }

    public void update(){
        HashMap<ObjectOfInterest,DemandUnit> tmp = new HashMap<ObjectOfInterest,DemandUnit>();
        if(du!=null)
            for(DemandUnit d : du){
                if(d.dem.getText()!=""){
                    try{
                        tmp.put(d.o,d);
                    }catch(Exception e){}
                }
            }
        du = new ArrayList<DemandUnit>();
        for(ObjectOfInterest o : con.objects){
            if(tmp.containsKey(o)){
                du.add(tmp.get(o));
            }else{
                du.add(new DemandUnit(o));
            }
        }

        removeAll();
        for(DemandUnit d : du){
            add(d);
        }
        add(new UpdateBtn(du,con));
        repaint();

    }

    public class UpdateBtn extends JButton{
        ArrayList<DemandUnit> al;
        MLBController c;
        public UpdateBtn(ArrayList<DemandUnit> al,MLBController c){
            this.al = al;
            this.c = c;
            setText("Update Demand");
            setSize(50,50);
            addActionListener(new ActionListener(){
                public void actionPerformed(ActionEvent e){
                    pushDemand();
                }
            });
        }
        public void pushDemand(){
            for(DemandUnit o : al){
                Double d = 0.0;
                try{
                    d = Double.parseDouble(o.dem.getText());
                }catch(Exception e){

                }
                c.demand.put(o.o,d);
            }
            for(ObjectOfInterest ooi : c.demand.keySet()){
                System.out.println(ooi.toString() + ":" + c.demand.get(ooi));
            }
        }
    }
    public class DemandUnit extends JPanel{

        JLabel name;
        JLabel txt;
        JTextField dem = new JTextField();
        ObjectOfInterest o;
        public DemandUnit(ObjectOfInterest o){
            this.o = o;
            name = new JLabel(o.name);
            txt = new JLabel("Demand:");

            setLayout(new BorderLayout());
            add(name,BorderLayout.NORTH);
            add(txt,BorderLayout.WEST);
            add(dem,BorderLayout.CENTER);
        }

    }

}
