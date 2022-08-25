package eskit.sdk.support.messenger.core;

import java.util.concurrent.SynchronousQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Create by WeiPeng on 2020/08/27 16:29
 */

public class Executors {

    private static final class ExecutorsHolder {
        private static final ThreadPoolExecutor sCommonExecutor = new SdkExecutor("mini");
    }

    public static ThreadPoolExecutor get() {
        return ExecutorsHolder.sCommonExecutor;
    }

    public static void destroy() {
//        ExecutorsHolder.sCommonExecutor.shutdownNow();
    }

    private static final class SdkExecutor extends ThreadPoolExecutor {

        public SdkExecutor(final String tag) {
            super(0, Integer.MAX_VALUE, 5L, TimeUnit.SECONDS, new SynchronousQueue<Runnable>(), new ThreadFactory() {
                private final AtomicInteger mCount = new AtomicInteger(1);

                @Override
                public Thread newThread(Runnable runnable) {
                    return new Thread(runnable, String.format("%s # %d", tag, this.mCount.getAndIncrement()));
                }
            });
        }
    }

    private Executors(){}

}
