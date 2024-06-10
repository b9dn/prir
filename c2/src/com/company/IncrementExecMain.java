package com.company;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

public class IncrementExecMain {
    public static int val = 0;

    public static void main(String[]args)throws Exception {
        int n = args.length > 0 ? Integer.parseInt(args[0]) : 10000;

        ExecutorService executor = Executors.newFixedThreadPool(n);


        Runnable runnableTask = () -> {
            synchronized (IncrementExecMain.class) {
                val++;
            }
        };
        ExecutorService[] executors = new ExecutorService[n];
        for(int i = 0; i < n; i++) {
            executors[i] = Executors.newSingleThreadExecutor();
            executors[i].execute(runnableTask);
        }

        executor.shutdown();
        try {
            if (!executor.awaitTermination(800, TimeUnit.MILLISECONDS)) {
                executor.shutdownNow();
            }
        } catch (InterruptedException e) {
            executor.shutdownNow();
        }

        System.out.println(val);
    }
}
