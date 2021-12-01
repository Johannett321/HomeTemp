package com.gruppe.arduinotest.pages;

import android.content.Context;
import android.graphics.drawable.Drawable;

import com.gruppe.arduinotest.R;

public class IconManager {

    public static Drawable getIconFromName(Context context, String iconName) {
        if (iconName.toLowerCase().contains("sove")) {
            return context.getResources().getDrawable(R.drawable.double_bed);
        }else if (iconName.toLowerCase().contains("stue")) {
            return context.getResources().getDrawable(R.drawable.living_room);
        }else if (iconName.toLowerCase().contains("bad")) {
            return context.getResources().getDrawable(R.drawable.bath);
        }
        return context.getResources().getDrawable(R.drawable.home);
    }
}
