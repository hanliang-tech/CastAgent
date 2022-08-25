package eskit.sdk.support.messenger.util;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.util.Log;
import android.util.Pair;

/**
 * Create by weipeng on 2022/06/03 11:34
 * Describe
 */
public class EsUtils {

    private static final String TAG = "[-EsUtils-]";

    private static final String TARGET_PACKAGE = "com.extscreen.runtime";

    private static final String AUTHORITIES = TARGET_PACKAGE + ".content.provider.EsContentProvider";

    public static boolean isEsRunning(Context context) {
        Uri uri = Uri.parse("content://" + AUTHORITIES + "/query/running");
        try (Cursor cursor =
                     context.getContentResolver().query(uri, null, null, null, null);
        ) {
            if (cursor != null && cursor.getCount() > 0) {
                cursor.moveToFirst();
                return cursor.getInt(0) > 0;
            }
        } catch (Exception e) {
            Log.w(TAG, "isEsRunning:" + e.getMessage());
        }
        return false;
    }

    public static Pair<String, Integer> getNetInfo(Context context) {
        Uri uri = Uri.parse("content://" + AUTHORITIES + "/query/net_info");
        try (Cursor cursor =
                     context.getContentResolver().query(uri, null, null, null, null);
        ) {
            if (cursor != null && cursor.getCount() > 0) {
                cursor.moveToFirst();
                return new Pair<>(
                        cursor.getString(0),
                        cursor.getInt(1)
                );
            }
        } catch (Exception e) {
            Log.w(TAG, "getNetInfo:" + e.getMessage());
        }
        return null;
    }

    public static void insertData(Context context, String ip, int port, String data) {
        Uri uri = Uri.parse("content://" + AUTHORITIES + "/insert");
        try {
            ContentValues values = new ContentValues();
            values.put("ip", ip);
            values.put("port", port);
            values.put("data", data);
            values.put("from", context.getPackageName());
            context.getContentResolver().insert(uri, values);
        } catch (Exception e) {
            Log.w(TAG, "insertData:" + e.getMessage());
        }
    }

}
