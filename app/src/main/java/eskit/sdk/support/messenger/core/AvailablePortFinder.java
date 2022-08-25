package eskit.sdk.support.messenger.core;

import java.io.IOException;
import java.net.DatagramSocket;
import java.net.ServerSocket;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.TreeSet;

public class AvailablePortFinder {

    public static final int MIN_PORT_NUMBER = 5000;
    public static final int MAX_PORT_NUMBER = 50000;

    private AvailablePortFinder() {
    }

    public static Set<Integer> getAvailablePorts() {
        return getAvailablePorts(MIN_PORT_NUMBER, MAX_PORT_NUMBER);
    }

    public static int getNextAvailable() {
        try {
            ServerSocket serverSocket = new ServerSocket(MIN_PORT_NUMBER);
            Throwable var1 = null;

            int var2;
            try {
                var2 = serverSocket.getLocalPort();
            } catch (Throwable var12) {
                var1 = var12;
                throw var12;
            } finally {
                if (serverSocket != null) {
                    if (var1 != null) {
                        try {
                            serverSocket.close();
                        } catch (Throwable var11) {
                            var1.addSuppressed(var11);
                        }
                    } else {
                        serverSocket.close();
                    }
                }

            }

            return var2;
        } catch (IOException var14) {
            throw new NoSuchElementException(var14.getMessage());
        }
    }

    public static int getNextAvailable(int fromPort) {
        if (fromPort >= MIN_PORT_NUMBER && fromPort <= MAX_PORT_NUMBER) {
            for(int i = fromPort; i <= MAX_PORT_NUMBER; ++i) {
                if (available(i)) {
                    return i;
                }
            }

            throw new NoSuchElementException("Could not find an available port above " + fromPort);
        } else {
            throw new IllegalArgumentException("Invalid start port: " + fromPort);
        }
    }

    public static boolean available(int port) {
        if (port >= MIN_PORT_NUMBER && port <= MAX_PORT_NUMBER) {
            ServerSocket ss = null;
            DatagramSocket ds = null;

            try {
                ss = new ServerSocket(port);
                ss.setReuseAddress(true);
                ds = new DatagramSocket(port);
                ds.setReuseAddress(true);
                boolean var3 = true;
                return var3;
            } catch (IOException var13) {
            } finally {
                if (ds != null) {
                    ds.close();
                }

                if (ss != null) {
                    try {
                        ss.close();
                    } catch (IOException var12) {
                    }
                }

            }

            return false;
        } else {
            throw new IllegalArgumentException("Invalid start port: " + port);
        }
    }

    public static Set<Integer> getAvailablePorts(int fromPort, int toPort) {
        if (fromPort >= MIN_PORT_NUMBER && toPort <= MAX_PORT_NUMBER && fromPort <= toPort) {
            Set<Integer> result = new TreeSet();

            for(int i = fromPort; i <= toPort; ++i) {
                ServerSocket s = null;

                try {
                    s = new ServerSocket(i);
                    result.add(i);
                } catch (IOException var14) {
                } finally {
                    if (s != null) {
                        try {
                            s.close();
                        } catch (IOException var13) {
                        }
                    }

                }
            }

            return result;
        } else {
            throw new IllegalArgumentException("Invalid port range: " + fromPort + " ~ " + toPort);
        }
    }
}