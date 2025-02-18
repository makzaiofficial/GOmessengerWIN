/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lang/lang_cloud_manager.h"

#include "lang/lang_instance.h"
#include "lang/lang_file_parser.h"
#include "lang/lang_text_entity.h"
#include "mtproto/mtp_instance.h"
#include "storage/localstorage.h"
#include "core/application.h"
#include "main/main_account.h"
#include "main/main_domain.h"
#include "ui/boxes/confirm_box.h"
#include "ui/wrap/padding_wrap.h"
#include "ui/widgets/labels.h"
#include "ui/text/text_utilities.h"
#include "core/file_utilities.h"
#include "core/click_handler_types.h"
#include "boxes/abstract_box.h" // Ui::hideLayer().
#include "styles/style_layers.h"

namespace Lang {
    namespace {

        class ConfirmSwitchBox : public Ui::BoxContent {
        public:
            ConfirmSwitchBox(
                QWidget*,
                const MTPDlangPackLanguage& data,
                Fn<void()> apply);

        protected:
            void prepare() override;

        private:
            QString _name;
            int _percent = 0;
            bool _official = false;
            QString _editLink;
            Fn<void()> _apply;

        };

        class NotReadyBox : public Ui::BoxContent {
        public:
            NotReadyBox(
                QWidget*,
                const MTPDlangPackLanguage& data);

        protected:
            void prepare() override;

        private:
            QString _name;
            QString _editLink;

        };

        ConfirmSwitchBox::ConfirmSwitchBox(
            QWidget*,
            const MTPDlangPackLanguage& data,
            Fn<void()> apply)
            : _name(qs(data.vnative_name()))
            , _percent(data.vtranslated_count().v * 100 / data.vstrings_count().v)
            , _official(data.is_official())
            , _editLink(qs(data.vtranslations_url()))
            , _apply(std::move(apply)) {
        }

        void ConfirmSwitchBox::prepare() {
            setTitle(tr::lng_language_switch_title());

            auto text = (_official
                ? tr::lng_language_switch_about_official
                : tr::lng_language_switch_about_unofficial)(
                    lt_lang_name,
                    rpl::single(Ui::Text::Bold(_name)),
                    lt_percent,
                    rpl::single(Ui::Text::Bold(QString::number(_percent))),
                    lt_link,
                    tr::lng_language_switch_link() | Ui::Text::ToLink(_editLink),
                    Ui::Text::WithEntities);
            const auto content = Ui::CreateChild<Ui::PaddingWrap<Ui::FlatLabel>>(
                this,
                object_ptr<Ui::FlatLabel>(
                    this,
                    std::move(text),
                    st::boxLabel),
                QMargins{ st::boxPadding.left(), 0, st::boxPadding.right(), 0 });
            content->entity()->setLinksTrusted();

            addButton(tr::lng_language_switch_apply(), [=] {
                const auto apply = _apply;
                closeBox();
                apply();
                });
            addButton(tr::lng_cancel(), [=] { closeBox(); });

            content->resizeToWidth(st::boxWideWidth);
            content->heightValue(
            ) | rpl::start_with_next([=](int height) {
                setDimensions(st::boxWideWidth, height);
                }, lifetime());
        }

        NotReadyBox::NotReadyBox(
            QWidget*,
            const MTPDlangPackLanguage& data)
            : _name(qs(data.vnative_name()))
            , _editLink(qs(data.vtranslations_url())) {
        }

        void NotReadyBox::prepare() {
            setTitle(tr::lng_language_not_ready_title());

            auto text = tr::lng_language_not_ready_about(
                lt_lang_name,
                rpl::single(_name) | Ui::Text::ToWithEntities(),
                lt_link,
                tr::lng_language_not_ready_link() | Ui::Text::ToLink(_editLink),
                Ui::Text::WithEntities);
            const auto content = Ui::CreateChild<Ui::PaddingWrap<Ui::FlatLabel>>(
                this,
                object_ptr<Ui::FlatLabel>(
                    this,
                    std::move(text),
                    st::boxLabel),
                QMargins{ st::boxPadding.left(), 0, st::boxPadding.right(), 0 });
            content->entity()->setLinksTrusted();

            addButton(tr::lng_box_ok(), [=] { closeBox(); });

            content->resizeToWidth(st::boxWidth);
            content->heightValue(
            ) | rpl::start_with_next([=](int height) {
                setDimensions(st::boxWidth, height);
                }, lifetime());
        }

    } // namespace

    Language ParseLanguage(const MTPLangPackLanguage& data) {
        return data.match([](const MTPDlangPackLanguage& data) {
            return Language{
                qs(data.vlang_code()),
                qs(data.vplural_code()),
                qs(data.vbase_lang_code().value_or_empty()),
                qs(data.vname()),
                qs(data.vnative_name())
            };
            });
    }

    CloudManager::CloudManager(Instance& langpack)
        : _langpack(langpack) {
        // Инициализация локального языкового пакета
        performSwitchToCustom();
    }

    Pack CloudManager::packTypeFromId(const QString& id) const {
        if (id == LanguageIdOrDefault(_langpack.id())) {
            return Pack::Current;
        }
        else if (id == _langpack.baseId()) {
            return Pack::Base;
        }
        return Pack::None;
    }

    rpl::producer<> CloudManager::languageListChanged() const {
        return _languageListChanged.events();
    }

    rpl::producer<> CloudManager::firstLanguageSuggestion() const {
        return _firstLanguageSuggestion.events();
    }

    void CloudManager::requestLangPackDifference(const QString& langId) {
        // Отключено: загрузка с сервера
    }

    void CloudManager::requestLangPackDifference(Pack pack) {
        // Отключено: загрузка с сервера
    }

    void CloudManager::setSuggestedLanguage(const QString& langCode) {
        // Отключено: предложение языка с сервера
    }

    void CloudManager::setCurrentVersions(int version, int baseVersion) {
        // Отключено: обновление версий с сервера
    }

    void CloudManager::applyLangPackDifference(
        const MTPLangPackDifference& difference) {
        // Отключено: применение обновлений с сервера
    }

    void CloudManager::requestLanguageList() {
        // Отключено: запрос списка языков с сервера
    }

    void CloudManager::offerSwitchLangPack() {
        // Отключено: предложение смены языка с сервера
    }

    Language CloudManager::findOfferedLanguage() const {
        return {}; // Отключено: поиск языка с сервера
    }

    bool CloudManager::showOfferSwitchBox() {
        return false; // Отключено: показ диалога смены языка
    }

    void CloudManager::applyLangPackData(
        Pack pack,
        const MTPDlangPackDifference& data) {
        // Отключено: применение данных с сервера
    }

    bool CloudManager::canApplyWithoutRestart(const QString& id) const {
        return true; // Всегда можно применить без перезагрузки
    }

    void CloudManager::resetToDefault() {
        performSwitch(DefaultLanguage());
    }

    void CloudManager::switchToLanguage(const QString& id) {
        performSwitchToCustom(); // Всегда используем локальный файл
    }

    void CloudManager::switchWithWarning(const QString& id) {
        performSwitchToCustom(); // Всегда используем локальный файл
    }

    void CloudManager::requestLanguageAndSwitch(
        const QString& id,
        bool warning) {
        performSwitchToCustom(); // Всегда используем локальный файл
    }

    void CloudManager::sendSwitchingToLanguageRequest() {
        // Отключено: запрос данных языка с сервера
    }

    void CloudManager::switchToLanguage(const Language& data) {
        //performSwitchToCustom(); // Всегда используем локальный файл
    }

    void CloudManager::performSwitchToCustom() {
        //// Указываем путь к локальному файлу lang.string
        //const QString filePath = u"path/to/your/lang.string"_q; // Замените на реальный путь к файлу

        //// Загружаем языковой пакет из указанного файла
        //auto loader = Lang::FileParser(
        //    filePath,
        //    { tr::lng_sure_save_language.base });

        //if (loader.errors().isEmpty()) {
        //    _langpack.switchToCustomFile(filePath);
        //    Local::writeLangPack();
        //}
        //else {
        //    Ui::show(
        //        Ui::MakeInformBox(
        //            "Custom lang failed :(\n\nError: " + loader.errors()),
        //        Ui::LayerOption::KeepOther);
        //}
    }

    void CloudManager::switchToTestLanguage() {
        // Отключено: тестовый язык
    }

    void CloudManager::performSwitch(const Language& data) {
        // Отключено: переключение на другой язык
    }

    void CloudManager::performSwitchAndAddToRecent(const Language& data) {
        // Отключено: добавление в недавние
    }

    void CloudManager::performSwitchAndRestart(const Language& data) {
        // Отключено: перезагрузка после смены языка
    }

    void CloudManager::restartAfterSwitch() {
        // Отключено: перезагрузка после смены языка
    }

    void CloudManager::switchLangPackId(const Language& data) {
        // Отключено: смена ID языкового пакета
    }

    void CloudManager::changeIdAndReInitConnection(const Language& data) {
        // Отключено: изменение ID и переинициализация соединения
    }

    void CloudManager::resendRequests() {
        // Отключено: повторная отправка запросов
    }

    CloudManager& CurrentCloudManager() {
        auto result = Core::App().langCloudManager();
        Assert(result != nullptr);
        return *result;
    }

} // namespace Lang