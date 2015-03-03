from django.contrib import admin

# Register your models here.
from django.contrib.auth.admin import UserAdmin
from django.contrib.auth.models import User

from webapp.models import UserSettings, Notification, UtilityCompany, RatePlan, Territory

# Define an inline admin descriptor for UserSettings model
# which acts a bit like a singleton
class UserSettingsInline(admin.StackedInline):
    model = UserSettings
    can_delete = False
    verbose_name_plural = 'usersettings'

# Define a new User admin
class UserAdmin(UserAdmin):
    inlines = (UserSettingsInline, )
    
class NotificationAdmin(admin.ModelAdmin):
   list_display = ('pk','description',)
   
class UtilityCompanyAdmin(admin.ModelAdmin):
   list_display = ('pk','description',)
   
class RatePlanAdmin(admin.ModelAdmin):
   list_display = ('pk','description',)
   
class TerritoryAdmin(admin.ModelAdmin):
   list_display = ('pk','description',)

# Re-register UserAdmin
admin.site.unregister(User)
admin.site.register(User, UserAdmin)

admin.site.register(Notification, NotificationAdmin)

admin.site.register(UtilityCompany, UtilityCompanyAdmin)
admin.site.register(RatePlan, RatePlanAdmin)
admin.site.register(Territory, TerritoryAdmin)


