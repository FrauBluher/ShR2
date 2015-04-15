from django.contrib import admin

# Register your models here.
from django.contrib.auth.admin import UserAdmin
from django.contrib.auth.models import User

from webapp.models import UserSettings, UtilityCompany, RatePlan, Territory, DashboardSettings, IntervalNotification

# Define an inline admin descriptor for UserSettings model
# which acts a bit like a singleton
class UserSettingsInline(admin.StackedInline):
    model = UserSettings
    can_delete = False
    verbose_name_plural = 'usersettings'

class DashboardSettingsInline(admin.StackedInline):
	model = DashboardSettings
	can_delete = False
	verbose_name_plural = 'dashboardsettings'

# Define a new User admin
class UserAdmin(UserAdmin):
    inlines = (UserSettingsInline, DashboardSettingsInline, )
   
class UtilityCompanyAdmin(admin.ModelAdmin):
   list_display = ('pk','description',)
   
class RatePlanAdmin(admin.ModelAdmin):
   list_display = ('pk','description',)
   
class TerritoryAdmin(admin.ModelAdmin):
   list_display = ('pk','description',)

# Re-register UserAdmin
admin.site.unregister(User)
admin.site.register(User, UserAdmin)

admin.site.register(UtilityCompany, UtilityCompanyAdmin)
admin.site.register(RatePlan, RatePlanAdmin)
admin.site.register(Territory, TerritoryAdmin)
admin.site.register(IntervalNotification)


