package com.smackall.iyan3dPro.DownloadManager;


/**
 * Created by maniselvaraj on 15/4/15.
 */
public interface RetryPolicy {

    /**
     * Returns the current timeout (used for logging).
     */
    int getCurrentTimeout();

    /**
     * Returns the current retry count (used for logging).
     */
    int getCurrentRetryCount();

    /**
     * Return back off multiplier
     */
    float getBackOffMultiplier();


    void retry() throws RetryError;


}
