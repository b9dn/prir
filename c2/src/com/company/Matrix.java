package com.company;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

public class Matrix {
    public static int val = 0;

    public static void main(String[]args)throws Exception {
        int n = args.length > 0 ? Integer.parseInt(args[0]) : 100000;

        ExecutorService executor = Executors.newFixedThreadPool(n);


        Runnable runnableTask = () -> {
            val++;
        };

        executor.execute(() -> {
            for (int i = 0; i < n; i++) {
                synchronized (IncrementExecMain.class) {
                    val++;
                }
            }
        });
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
