package com.company;

class Increment extends Thread {

        public Increment() {}

        @Override
        public void run () {
            synchronized(Main.class) {
                //val++;
            }
        }
}