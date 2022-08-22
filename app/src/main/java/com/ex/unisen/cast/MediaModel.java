package com.ex.unisen.cast;

import android.os.Parcel;
import android.os.Parcelable;

public class MediaModel implements Parcelable {

	private String uri = "";
	private String title = "";
	private String artist = "";
	private String album = "";
	private String albumiconuri = "";
	private String objectclass = "";


	public String getTitle() {
		return title;
	}
	public void setTitle(String title) {
		this.title = (title != null ? title : "");
	}

	public String getArtist() {
		return artist;
	}
	public void setArtist(String artist) {
		this.artist = (artist != null ? artist : "");
	}

	public void setAlbum(String album) {
		this.album = (album != null ? album : "");
	}
	public String getAlbum() {
		return album;
	}

	public void setObjectClass(String objectClass) {
		this.objectclass = (objectClass != null ? objectClass : "");
	}
	public String getObjectClass() {
		return objectclass;
	}

	public void setUrl(String uri) {
		this.uri = (uri != null ? uri : "");
	}
	public String getUrl() {
		return uri;
	}

	public String getAlbumUri(){
		return albumiconuri;
	}
	public void setAlbumUri(String albumiconuri){
		this.albumiconuri = (albumiconuri != null ? albumiconuri : "");
	}


	@Override
	public int describeContents() {
		return 0;
	}

	@Override
	public void writeToParcel(Parcel dest, int flags) {
		dest.writeString(this.uri);
		dest.writeString(this.title);
		dest.writeString(this.artist);
		dest.writeString(this.album);
		dest.writeString(this.albumiconuri);
		dest.writeString(this.objectclass);
	}

	public MediaModel() {
	}

	protected MediaModel(Parcel in) {
		this.uri = in.readString();
		this.title = in.readString();
		this.artist = in.readString();
		this.album = in.readString();
		this.albumiconuri = in.readString();
		this.objectclass = in.readString();
	}

	public static final Parcelable.Creator<MediaModel> CREATOR = new Parcelable.Creator<MediaModel>() {
		@Override
		public MediaModel createFromParcel(Parcel source) {
			return new MediaModel(source);
		}

		@Override
		public MediaModel[] newArray(int size) {
			return new MediaModel[size];
		}
	};
}
