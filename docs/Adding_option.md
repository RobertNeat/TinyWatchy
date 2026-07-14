# Adding a menu option

Menu features implement `AbstractOption`. Instances are owned by `Menu` and referenced from its `_pages` structure.

## Interface

Implement every method in `AbstractOption`:

```cpp
class ExampleOption : public AbstractOption {
public:
    std::string getTitle() override;
    std::string getDescription(const StackPage &stackPage) override;
    void onNextButtonPressed() override;
    void onPrevButtonPressed() override;
    bool onSelectButtonPressed(const StackPage &stackPage) override;
    void onBackButtonPressed() override;
};
```

- `getTitle()` returns the menu title.
- `getDescription()` returns the current value, prompt, or status. `StackPage::selected` indicates edit mode.
- `onNextButtonPressed()` and `onPrevButtonPressed()` are called while the option is selected.
- `onSelectButtonPressed()` returns `true` to enter selected/edit mode or `false` for an immediate action.
- `onBackButtonPressed()` lets the option cancel or reset temporary state. `Menu` itself leaves edit mode or returns to the parent page.

Options provide text and behavior; watch faces perform the actual drawing. Keep titles and descriptions short enough for the included faces. The current UI test option can be used to check layout.

## Registration

1. Add the option header, and a `.cpp` file when needed, under `src/MenuOptions/`.
2. Include it from `src/Menu.h`.
3. Add an instance as a `Menu` member. Put required dependency pointers in its constructor rather than using globals.
4. Initialize the instance in the `Menu` constructor initializer list.
5. Add its address to the appropriate `MenuPage::items` list in `Menu::_pages`.

Example:

```cpp
ExampleOption _exampleOption;

const MenuPage _pages[3] = {
    {
        .items = {
            &_menuOption,
            &_ntpOption,
            &_exampleOption,
            &_settingsSubmenu,
        },
    },
    // Remaining pages...
};
```

Use `SubMenuOption` with a callback to `changePage()` when the feature needs a separate page. If another service is required by the option, pass it through `Menu` from `TinyWatchy` and update the relevant constructors.
