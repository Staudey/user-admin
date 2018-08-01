#include "user-base.h"
#include "user-password.h"
#include "user-share.h"

/******************************************************************************
* Function:            SwitchState 
*        
* Explain: Select auto login,Only one user can choose to log in automatically.
*        
* Input:         
*        
* Output: 
*        
* Author:  zhuyaliang  09/05/2018
******************************************************************************/
static void SwitchState(GtkSwitch *widget,gboolean   state,gpointer  data)
{

    GSList *list;
    GSList *l;
    int i =0 ;
    UserAdmin *ua = (UserAdmin *)data;
    
    if(Change == 0)
    {        
        ActUser *user = ua->ul[gnCurrentUserIndex].User;
        ActUserManager *um =  act_user_manager_get_default ();
        if(state == TRUE)
        {
            list = act_user_manager_list_users (um);
            for (l = list; l != NULL; l = l->next,i++)
            {
                ActUser *u = l->data;
                ua->ul[i].LoginType = FALSE;
                if (act_user_get_uid (u) != act_user_get_uid (user)) 
                {
                    act_user_set_automatic_login (user, FALSE);
                }
            }
            g_slist_free (list);
            act_user_set_automatic_login(user,TRUE);
        }
        else
            act_user_set_automatic_login(user,FALSE);
        
        ua->ul[gnCurrentUserIndex].LoginType = state; 

    }
}    
/******************************************************************************
* Function:             ChangePass 
*        
* Explain: Modifying the cipher signal.The two state .Change the password 
*          .Set set the new password.
* Input:         
*        
* Output: 
*        
* Author:  zhuyaliang  15/05/2018
******************************************************************************/
static void ChangePass(GtkWidget *widget,gpointer data)
{
    UserAdmin *ua = (UserAdmin *)data;
	//const char *s = gtk_button_get_label(GTK_BUTTON(widget));
    gtk_widget_set_sensitive(ua->MainWindow,FALSE);

    if(ua->ul[gnCurrentUserIndex].PasswordType == OLDPASS)
		ChangeOldPass(ua);      //old passwoed change
	else
		CreateNewPass(ua);      //There is no password for the user

}
/******************************************************************************
* Function:              ComboSelectLanguage 
*        
* Explain: Switch language signal
*        
* Input:         
*        
* Output: 
*        
* Author:  zhuyaliang  09/05/2018
******************************************************************************/
static void ComboSelectLanguage(GtkWidget *widget,gpointer data)
{
    UserAdmin *ua = (UserAdmin *)data;
    gint account_type;
   
    if(Change == 0) 
    {        
        account_type =  gtk_combo_box_get_active (GTK_COMBO_BOX(widget));
        ua->ul[gnCurrentUserIndex].LangType = account_type;
        act_user_set_language(ua->ul[gnCurrentUserIndex].User,all_languages[account_type]);
    }     
}
/******************************************************************************
* Function:             ComboSelectUserType 
*        
* Explain: Select user type signal
*        
* Input:         
*        
* Output: 
*        
* Author:  zhuyaliang  09/05/2018
******************************************************************************/
static void ComboSelectUserType(GtkWidget *widget,gpointer data)
{
    UserAdmin *ua = (UserAdmin *)data;
    gint account_type;

    if( Change ==0 )
    {
        account_type =  gtk_combo_box_get_active (GTK_COMBO_BOX(widget)) ? 
                                                  ACT_USER_ACCOUNT_TYPE_ADMINISTRATOR:
                                                  ACT_USER_ACCOUNT_TYPE_STANDARD;
        ua->ul[gnCurrentUserIndex].UserType = account_type;
        act_user_set_account_type(ua->ul[gnCurrentUserIndex].User,account_type);
    }    
}
/******************************************************************************
* Function:              DisplayUserSetOther 
*        
* Explain: Displays user type and user language and user password, 
*          and automatic login and logon time.
*        
* Input:         
*        
* Output: 
*        
* Author:  zhuyaliang  09/05/2018
******************************************************************************/
void DisplayUserSetOther(GtkWidget *Hbox,UserAdmin *ua)
{
    GtkWidget *table;
    GtkWidget *fixed;
    GtkWidget *ButtonPass;
    GtkWidget *LabelUserType;
    GtkWidget *LabelAutoLogin;
    GtkWidget *LabelLanguage;
    GtkWidget *SwitchLogin;
    GtkWidget *LabelPass;
    GtkWidget *ComboLanguage;
    GtkWidget *LabelTime;
    GtkWidget *ButtonTime;
    GtkWidget *ComboUser;

    fixed = gtk_fixed_new();
    gtk_box_pack_start(GTK_BOX(Hbox),fixed ,TRUE, TRUE, 0);
    table = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(table),TRUE);
    gtk_fixed_put(GTK_FIXED(fixed), table, 0, 0);

    /*user type*/
    LabelUserType = gtk_label_new(NULL);
    SetLableFontType(LabelUserType,"gray",11,_("Account Type"));
    gtk_grid_attach(GTK_GRID(table) , LabelUserType , 0 , 0 , 1 , 1);

    /*drop-down select boxes*/
    ComboUser = SetComboUserType(_("Standard"),_("Administrators"));
    ua->ComUserType = ComboUser; 
    gtk_combo_box_set_active(GTK_COMBO_BOX(ComboUser),ua->ul[0].UserType);
    gtk_grid_attach(GTK_GRID(table) , ComboUser , 1 , 0 , 2 , 1);
    g_signal_connect(G_OBJECT(ComboUser),
                    "changed",
                    G_CALLBACK(ComboSelectUserType),
                    ua);

   /*select langusge*/ 
    LabelLanguage = gtk_label_new(NULL);
    SetLableFontType(LabelLanguage,"gray",11,_("Language"));
    gtk_grid_attach(GTK_GRID(table) , LabelLanguage , 0 , 1 , 1 , 1);

    ComboLanguage = SetComboLanguageType();
    ua->ComUserLanguage = ComboLanguage;
    gtk_combo_box_set_active(GTK_COMBO_BOX(ComboLanguage),ua->ul[0].LangType);
    gtk_grid_attach(GTK_GRID(table) , ComboLanguage , 1 , 1 , 2 , 1);
    g_signal_connect(G_OBJECT(ComboLanguage),
                    "changed",
                    G_CALLBACK(ComboSelectLanguage),
                    ua);
    
    /*set password*/
    LabelPass = gtk_label_new(NULL);
    SetLableFontType(LabelPass,"gray",11,_("Password"));
    gtk_grid_attach(GTK_GRID(table) , LabelPass , 0 , 2 , 1 , 1);
    
    ButtonPass = gtk_button_new_with_label(ua->ul[0].PassText);
    ua->ButtonPass = ButtonPass;
    g_signal_connect (ButtonPass, "clicked",G_CALLBACK (ChangePass),ua);
    gtk_grid_attach(GTK_GRID(table) , ButtonPass , 1 , 2 , 2 , 1);
   
    /*auto login*/
    LabelAutoLogin = gtk_label_new(NULL);
    SetLableFontType(LabelAutoLogin,"gray",11,_("Automatic logon"));
    gtk_grid_attach(GTK_GRID(table) , LabelAutoLogin , 0 , 3 , 1 , 1);
    
    SwitchLogin = gtk_switch_new();
    ua->SwitchAutoLogin = SwitchLogin;
    gtk_switch_set_state (GTK_SWITCH(SwitchLogin),ua->ul[0].LoginType);
    gtk_grid_attach(GTK_GRID(table) , SwitchLogin , 1 , 3 , 1 , 1);
    g_signal_connect(G_OBJECT(SwitchLogin),
                    "state-set",
                    G_CALLBACK(SwitchState),
                    ua);
    
    /*login time*/
    LabelTime = gtk_label_new(NULL);
    SetLableFontType(LabelTime,"gray",11,_("Login time"));
    gtk_grid_attach(GTK_GRID(table) , LabelTime, 0 , 4 , 1 , 1);
  
    ButtonTime = gtk_button_new_with_label (ua->ul[0].UserTime);
    ua->ButtonUserTime = ButtonTime;
    gtk_grid_attach(GTK_GRID(table) , ButtonTime, 1 , 4 , 2 , 1);
  
    gtk_grid_set_row_spacing(GTK_GRID(table), 10);
    gtk_grid_set_column_spacing(GTK_GRID(table), 10);
}
