//
//  SuperCanvas.java
//  GoBible
//
//	Go Bible is a Free Bible viewer application for Java mobile phones (J2ME MIDP 1.0 and MIDP 2.0).
//	Copyright © 2003-2008 Jolon Faichney.
//	Copyright © 2008-2009 CrossWire Bible Society.
//
//	This program is free software; you can redistribute it and/or
//	modify it under the terms of the GNU General Public License
//	as published by the Free Software Foundation; either version 2
//	of the License, or (at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

import javax.microedition.lcdui.*;
import java.util.Timer;
import java.util.TimerTask;

/**
 *
 * @author user
 *
 * SuperCanvas class To separate out the additional touch-capable functions
 * 
 */
public abstract class SuperCanvas extends Canvas {
    private int popupDelay = 500;
    private int staticAllowance = 0;
    public int getPopupDelay() { return popupDelay; }
    public void setPopupDelay(int p) { this.popupDelay = p; }

    private Timer timer;
    private SuperCanvasTimerTask timerTask = null;
    protected boolean cancelled = true;
    
    public SuperCanvas() {
        timer = new Timer();

        initializeSpeedometer();
    }

    protected abstract void pointerHeld(int x, int y);

    private int firstX, firstY;

    protected void pointerPressed(int x, int y) {
        firstX = x;
        firstY = y;
        //System.err.println("[Super Canvas] Pointer Pressed");
        timerTask =  new SuperCanvasTimerTask(this, x, y);
        timer.scheduleAtFixedRate(timerTask, popupDelay, popupDelay);

        s_pointerPressed(x, y); // implementation for kinetic scrolling
    }
    protected void pointerDragged(int x, int y) {
        /* If the requirement of not moving a finger is too strict,
         * allow some leeway by giving an allowance within which the
         * pointer is considered to have NOT moved.
        */
        if ( (firstX - x)*(firstX - x) + (firstY - y)*(firstY - y) <= staticAllowance * staticAllowance ) {
            return;
        }
        //System.err.println("[Super Canvas] Pointer Dragged");
        if (timerTask != null) {
            timerTask.cancel();
            timerTask = null;
        }
        //timerTask.successfulTicks = 0;

        s_pointerDragged(x, y); // implementation for kinetic scrolling
    }
    protected void pointerReleased(int x, int y) {
        //System.err.println("[Super Canvas] Pointer Released");
        if (timerTask != null) {
            timerTask.cancel();
        }
        s_pointerReleased(x, y); // implementation for kinetic scrolling
    }
    protected void setStaticAllowance(int k) {
        staticAllowance = k;
    }
    protected int getStaticAllowance() {
        return staticAllowance;
    }


    /* speedometer methods and fields */
    private long lastTime;
    private int lastX, lastY;
    private double speedX, speedY;
    public static final double RESISTANCE = 0.9;
    //public static final double RESISTANCE = 0.005;
    public static final double CUTOFF = 0.01; // speed below which object faces infinity resistance
    public static final int TIMER_INTERVAL = 30; // 1000 / 50 = 20x per sec
    public static final int SAMPLE_INTERVAL = 10; // 1000 / 50 = 20x per sec
    public static final int WAIT_TIME = 200;

    private TimerTask kineticTimerTask = null;

    private void initializeSpeedometer() {
        lastTime = System.currentTimeMillis();
    }
    private void s_pointerPressed(int x, int y) {
        // first{X,Y} have already been saved in pointerPressed
        lastX = x;
        lastY = y;
        lastTime = System.currentTimeMillis();
        if (kineticTimerTask != null) {
            kineticTimerTask.cancel();
        }
    }
    private void s_pointerDragged(int x, int y) {
        long timeInterval = System.currentTimeMillis() - lastTime;
        int diffX, diffY;
        int dispX, dispY;

        if (timeInterval < SAMPLE_INTERVAL) {
            // If the phone is THAT responsive, issuing
            // 2x pointerDragged in the same millisec,
            return; // Skip
        }

        dispX = x - firstX;
        dispY = y - firstY;

        diffX = (x - lastX);
        diffY = (y - lastY);

        speedX = ((double) diffX) / timeInterval;
        speedY = ((double) diffY) / timeInterval;

        pointerMovingRelative(x, y, diffX, diffY);

        lastX = x;
        lastY = y;

        lastTime += timeInterval; // Either add the difference or = system.currentTimeInMillis()
    }
    private void s_pointerReleased(final int x, final int y) {
        long timeInterval = System.currentTimeMillis() - lastTime;
        
        if (-CUTOFF < speedX && speedX < CUTOFF
                && -CUTOFF < speedY && speedY < CUTOFF || timeInterval > WAIT_TIME) {
            // Final speed is too low (< CUTOFF),
            // or user has held pointer at the last position for a while (WAIT_TIME).
            // Do not continue to scroll.
            return;
        }

        timer.schedule( kineticTimerTask = new TimerTask() {
            public void run() {
                // exponential slowing ("air resistance")
                
                speedX *= RESISTANCE;
                speedY *= RESISTANCE;
                if ((int) (speedX * TIMER_INTERVAL + 0.5) == 0) {
                    speedX = 0;
                }
                if ((int) (speedY * TIMER_INTERVAL + 0.5) == 0) {
                    //System.err.println("Effectively zero speed: " + speedY);
                    speedY = 0;
                }
                
                if (speedX == 0 && speedY == 0) {
                    // already not moving, so kill the timer
                    this.cancel();
                    kineticTimerTask = null;
                    return;
                }
                pointerMovingRelative(x, y, (int) speedX * TIMER_INTERVAL, (int) speedY * TIMER_INTERVAL);
            }
        }, 0, TIMER_INTERVAL);
    }

    protected abstract void pointerMovingRelative(int x, int y, int delX, int delY);
}

class SuperCanvasTimerTask extends TimerTask {
    private int x, y;
    private SuperCanvas sc;
    protected boolean executed = false;
    protected int successfulTicks = 0;

    public final static int requiredSuccessfulTicks = 2;

    protected SuperCanvasTimerTask(SuperCanvas sc, int x, int y) {
        this.x = x;
        this.y = y;
        this.sc = sc;
    }
    public void run() {
        if (successfulTicks < requiredSuccessfulTicks) {
            successfulTicks++;
            return;
        }
        this.cancel();
        sc.pointerHeld(x, y);
    }
}
