# M3 面向对象设计说明

## 一、目标

M3 在 M2 的基础战斗闭环上增加五种植物、三种僵尸、多波次关卡和图鉴。
核心规则保留在 `object_project_core` 中，SFML 界面只读取规则状态并处理输入，
避免把伤害、护甲和胜负判断写进绘制代码。

## 二、主要类关系

```text
Entity
├─ Plant
│  ├─ Sunflower
│  ├─ Peashooter
│  ├─ WallNut
│  ├─ Repeater
│  └─ Threepeater
└─ Zombie
   ├─ NormalZombie
   ├─ ConeheadZombie
   └─ BucketheadZombie

GameSession
├─ Board ───────── owns Plant objects
├─ CardSystem ──── owns costs and cooldowns
├─ SunSystem ───── owns collectible sun tokens
├─ CombatSystem ── owns Zombie objects, projectiles and lawn mowers
└─ LevelSystem ─── emits typed spawn events in three waves

GameScreen ─────── draws and controls the active game
AlmanacScreen ──── draws browsable plant and zombie information
AlmanacData ────── provides immutable descriptions for the almanac
```

## 三、封装、继承、多态和组合

- **封装**：`Zombie::receiveDamage` 负责“先扣护甲、再扣生命值”，调用方不直接改护甲。
- **继承**：五种植物继承 `Plant`，三种僵尸继承 `Zombie`，共同状态由基类保存。
- **多态**：`Plant::update` 返回不同攻击动作；`Zombie` 的速度、伤害和攻击间隔由具体类型实现。
- **组合**：`GameSession` 组合棋盘、卡片、阳光、战斗和关卡系统，统一控制暂停、重开和胜负。

`createPlant` 与 `createZombie` 是工厂函数。工厂函数就是根据类型创建正确对象的统一入口，
这样棋盘和战斗系统不需要知道每个具体类的构造细节。

## 四、关键规则

- 双发射手一次产生两枚同路子弹；三线射手向本行和有效相邻行各产生一枚子弹。
- 路障和铁桶护甲耗尽后，后续伤害才减少僵尸本体生命值。
- `LevelSystem` 的生成事件同时记录时间、行、僵尸类型和波次。
- 最后一波全部生成不等于胜利；`GameSession` 还会等待场上僵尸全部被清除。
- 打开图鉴时主循环停止更新游戏，但保留当前战局，返回后继续。

## 五、扩展边界

M4 可以继续增加减速、爆炸、越过和狂暴行为。新增类型应优先扩展植物动作、僵尸虚函数
或生成配置，不应把类型判断散落到输入、绘制和胜负代码中。M3 的装甲颜色和新植物卡片
使用程序绘制的占位外观，后续有许可证明确的素材时再替换。
