package com.hepta.vcn;

import org.junit.Test;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class ExampleUnitTest {
    @Test
    public void addition_isCorrect() {
//        assertEquals(4, 2 + 2);
        System.out.println("fewfewfew");

        indicatorThreadPool.execute(()->{
            try{
                System.out.println("线程一执行");
            }catch (Exception e) {
                System.out.println(e.getMessage());
            }

        });
        indicatorThreadPool.execute(() -> {

            for (int i=0;i<10;i++) {
                try {
                    System.out.println("线程二执行");
                    Thread.sleep(1000);
                } catch (Exception e) {
                    System.out.println(e.getMessage());
                }
            }

        });
        indicatorThreadPool.shutdown();

        try{
            indicatorThreadPool.awaitTermination(30 , TimeUnit.SECONDS);
            System.out.println("end");
        }catch (Exception e){
            e.printStackTrace();
        }

    }

    private final ExecutorService indicatorThreadPool= Executors.newFixedThreadPool(5);

}