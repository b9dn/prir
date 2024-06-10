package com.company;

public class Main {
    public static int val = 0;

    static class Increment extends Thread {

        public Increment() {}

        @Override
        public void run () {
            synchronized(Main.class) {
                val++;
            }
        }
    }

    public static void main(String[]args)throws Exception {
        int n = args.length > 0 ? Integer.parseInt(args[0]) : 100000;
        Thread[] threads = new Thread[n];
        for(int i = 0; i < n; i++) {
            threads[i] = new Increment();
            threads[i].start();
        }

        for(int i = 0; i < n; i++) {
            threads[i].join();
        }

        System.out.println(val);
    }
}
