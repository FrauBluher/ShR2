from django.contrib import admin

# Register your models here.
from django.contrib.auth.admin import UserAdmin
from django.contrib.auth.models import User

from webapp.models import UserSettings, UtilityCompany, RatePlan, Territory, DashboardSettings, IntervalNotification, EventNotification, Tier

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
   list_display = ('description','pk',)

class TierInline(admin.StackedInline):
   """
   Class used to expose the :class:`webapp.models.Tier` model to the administrator. This was designed to be flexible to allow an administrator to add/modify tiers based on electric company data.
   """
   model = Tier
   
class RatePlanAdmin(admin.ModelAdmin):
   """
   Class used to expose the :class:`webapp.models.RatePlan` model to the administrator. This was designed to be flexible to allow an administrator to add/modify Rate Plans based on electric company data.
   """
   list_display = ('description','utility_company','pk',)
   inlines = (TierInline,)
   
class TerritoryAdmin(admin.ModelAdmin):
   """
   Class used to expose the :class:`webapp.models.Territory`` model to the administrator. This was designed to be flexible to allow an administrator to add/modify Territories based on electric company data.
   """
   list_display = ('description','rate_plan','pk',)

# Re-register UserAdmin
admin.site.unregister(User)
admin.site.register(User, UserAdmin)

admin.site.register(UtilityCompany, UtilityCompanyAdmin)
admin.site.register(RatePlan, RatePlanAdmin)
admin.site.register(Territory, TerritoryAdmin)
admin.site.register(IntervalNotification)
admin.site.register(EventNotification)

