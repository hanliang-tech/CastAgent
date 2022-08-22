package com.ex.unisen.cast;

import android.content.Intent;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import java.io.ByteArrayInputStream;
import java.io.InputStream;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

public class MediaModelFactory {


    public static final String PARAM_GET_URL = "param_metadata__url";
    public static final String PARAM_GET_OBJECT_CLASS = "param_metadata_object_class";
    public static final String PARAM_GET_TITLE = "param_metadata_title";
    public static final String PARAM_GET_ARTIST = "param_metadata_artist";
    public static final String PARAM_GET_ALBUM = "param_metadata_album";
    public static final String PARAM_GET_ALBUMICONURI = "param_metadata_album_uri";


    public static void pushMediaModelToIntent(Intent intent, MediaModel mediaInfo) {
        intent.putExtra(PARAM_GET_URL, mediaInfo.getUrl());
//		intent.putExtra(PARAM_GET_OBJECT_CLASS, mediaInfo.getObjectClass());
//		intent.putExtra(PARAM_GET_TITLE, mediaInfo.getTitle());
//		intent.putExtra(PARAM_GET_ARTIST, mediaInfo.getArtist());
//		intent.putExtra(PARAM_GET_ALBUM, mediaInfo.getAlbum());
//		intent.putExtra(PARAM_GET_ALBUMICONURI, mediaInfo.getAlbumUri()); 	
    }

    public static MediaModel createFromIntent(Intent intent) {
        MediaModel mediaInfo = new MediaModel();
        mediaInfo.setUrl(intent.getStringExtra(PARAM_GET_URL));
//		mediaInfo.setObjectClass(intent.getStringExtra(PARAM_GET_OBJECT_CLASS));
//		mediaInfo.setTitle(intent.getStringExtra(PARAM_GET_TITLE));
//		mediaInfo.setArtist(intent.getStringExtra(PARAM_GET_ARTIST));
//		mediaInfo.setAlbum(intent.getStringExtra(PARAM_GET_ALBUM));
//		mediaInfo.setAlbumUri(intent.getStringExtra(PARAM_GET_ALBUMICONURI));	
        return mediaInfo;
    }

    public static MediaModel createFromUrl(String url) {
        MediaModel mediaInfo = new MediaModel();
        mediaInfo.setUrl(url);
        return mediaInfo;
    }

    public static MediaModel createFromMetaData(String metadata) {
        MediaModel mediainfo = new MediaModel();
        DocumentBuilderFactory dfactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder documentBuilder;

        if (metadata.contains("&") && !metadata.contains("&amp;")) {
            metadata = metadata.replace("&", "&amp;");
        }

        try {
            documentBuilder = dfactory.newDocumentBuilder();
            InputStream is = new ByteArrayInputStream(metadata.getBytes("UTF-8"));
            Document doc = documentBuilder.parse(is);
            mediainfo.setObjectClass(getElementValue(doc, "upnp:class"));
            mediainfo.setTitle(getElementValue(doc, "dc:title"));
            mediainfo.setAlbum(getElementValue(doc, "upnp:album"));
            mediainfo.setArtist(getElementValue(doc, "upnp:artist"));
            mediainfo.setAlbumUri(getElementValue(doc, "upnp:albumArtURI"));
        } catch (Exception e) {
            e.printStackTrace();
        }
        return mediainfo;
    }

    private static String getElementValue(Document doc, String element) {
        NodeList containers = doc.getElementsByTagName(element);
        for (int j = 0; j < containers.getLength(); ++j) {
            Node container = containers.item(j);
            NodeList childNodes = container.getChildNodes();
            if (childNodes.getLength() != 0) {
                Node childNode = childNodes.item(0);
                return childNode.getNodeValue();
            }
        }
        return "";
    }
}



