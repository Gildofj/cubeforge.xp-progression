# Mathematical Formulas & Progression Curves

This document provides a comprehensive reference of all mathematical formulas, curves, and scaling algorithms implemented across **PyroProgression**.

---

## 🧭 Table of Contents
1. [Core Progression & XP Curves](#core-progression--xp-curves)
2. [World & Distance Metrics](#world--distance-metrics)
3. [Creature & Item Level Determination](#creature--item-level-determination)
4. [Player Stat Scaling](#player-stat-scaling)
5. [Creature Scaling Formula](#creature-scaling-formula)
6. [Gear Stat Calculations](#gear-stat-calculations)
7. [Economy & Gold Formulas](#economy--gold-formulas)
8. [Pseudo-Random Number Generator (PyroRand)](#pseudo-random-number-generator-pyrorand)

---

## Core Progression & XP Curves

### XP Required Per Level
The XP needed to reach the next level is calculated using an exponential power curve:

$$\text{XP}_{\text{required}}(\text{Level}) = 50 \times \left(1 + \text{Level}^{1.3}\right)$$

```
 Level  1: ~100 XP
 Level  5: ~455 XP
 Level 10: ~1,047 XP
 Level 25: ~3,334 XP
 Level 50: ~8,197 XP
 Level 100:~20,136 XP
```

### XP Gain from Kills
XP granted on creature death depends on the creature's level, star rating, and boss modifiers:

$$\text{XP}_{\text{gain}} = \text{Level} \times \text{Stars} \times M_{\text{boss}}$$

Where $\text{Stars} = \text{creature.entity\_data.level} + 1$, and $M_{\text{boss}}$ is defined as:

| Boss Type | Modifier ($M_{\text{boss}}$) | Appearance Bitflag |
|---|---|---|
| Normal Creature | $1.0\times$ | `None` |
| Mini Boss | $2.0\times$ | `AppearanceModifiers::IsMiniBoss` |
| Named Boss | $5.0\times$ | `AppearanceModifiers::IsNamedBoss` |
| World Boss | $10.0\times$ | `AppearanceModifiers::IsBoss` |

> [!NOTE]
> Modifiers can stack if multiple flags are active simultaneously.

---

## World & Distance Metrics

### Region Distance (Chebyshev Distance)
The distance of any region $(X_r, Y_r)$ from the player's home/base region $(X_0, Y_0)$ is computed using Chebyshev distance ($L_\infty$ norm):

$$D_{\text{region}} = \max\left(|X_0 - X_r|,\ |Y_0 - Y_r|\right)$$

This means the starting region has $D = 0$, the immediate 8 adjacent regions have $D = 1$, the ring surrounding those has $D = 2$, and so on.

---

## Creature & Item Level Determination

### Region Level Bounds
Each region spans a bracket of levels based on `LEVELS_PER_REGION = 5`:

$$\text{Level}_{\text{min}}(D) = 1 + 5 \times D$$
$$\text{Level}_{\text{max}}(D) = 5 \times (D + 1)$$

### Creature Level Calculation
For enemies (non-player / non-pet entities):

$$\text{Level}_{\text{creature}} = (1 + 5 \times D_{\text{region}}) + (\text{PyroRand}(\text{creature.id}) \bmod 5)$$

### Item Level Calculation
For equipment items:

$$\text{Level}_{\text{item}} = (1 + 5 \times D_{\text{region}}) + (\text{PyroRand}(\text{item.modifier}) \bmod 5)$$

### Level Equipment Cap
A player may only equip gear if:

$$\text{Level}_{\text{player}} + \text{LEVEL\_EQUIPMENT\_CAP} \ge \text{Level}_{\text{item}}$$

Currently, $\text{LEVEL\_EQUIPMENT\_CAP} = 0$, meaning players cannot equip items of a higher level than their current character level.

---

## Player Stat Scaling

Player base stats are adjusted dynamically upon level calculation:

$$\text{Stat}_{\text{final}} = (\text{Stat}_{\text{base}} - \text{BaseOffset}) + (\text{ScalingFactor} \times \text{Level})$$

| Stat Type | Base Offset | Scaling Factor / Level |
|---|---|---|
| **Health (HP)** | $-50.0$ | $+1.0000$ |
| **Attack Power** | $-2.0$ | $+0.2000$ |
| **Spell Power** | $-2.0$ | $+0.2000$ |
| **Armor** | $0.0$ | $+0.2000$ |
| **Resistance** | $0.0$ | $+0.2000$ |
| **Critical Strike** | $0.0$ | $+0.0001$ |
| **Haste** | $0.0$ | $+0.0001$ |
| **Stamina (Regen)** | $0.0$ | $+0.0500$ |
| **Mana Generation**| $0.0$ | $+0.0500$ |

---

## Creature Scaling Formula

Creatures scale non-linearly using a logarithmic multiplier that increases with creature level:

$$\text{Stat}_{\text{creature}} = \text{Stat}_{\text{base}} \times 0.05 \times (1 + S_{\text{type}}) \times \left(1 + 1000 \times \log_2\left(\frac{\text{Level} + 1001}{1000}\right)\right)$$

| Stat Type | Creature Scaling Factor ($S_{\text{type}}$) |
|---|---|
| **Health (HP)** | $2.00$ |
| **Attack Power** | $3.00$ |
| **Spell Power** | $3.00$ |
| **Armor** | $0.01$ |
| **Resistance** | $0.01$ |

---

## Gear Stat Calculations

### Base Weapon & Armor Scaling (`GetGearScaling`)
The base offensive/defensive values for player equipment are computed as:

$$\text{BaseRes} = \frac{\text{base} \times 0.5}{32.0}$$
$$\text{ModModifier} = \frac{\text{mod}_3 / 274877907}{8.0}$$
$$\text{Result}_{\text{base}} = |0.5 + \text{BaseRes}|$$

For player items:

$$\text{Stat}_{\text{gear}} = \text{Result}_{\text{base}} \times 1000 \times \log_2\left(\frac{\text{Level}_{\text{item}} + 3 \times (\text{EffectiveRarity} + \text{ModModifier}) + 1001}{1000}\right)$$

### Secondary Stats Scaling (`GetOtherStatsRe`)
Secondary stats (Crit, Haste, Regen) use a power curve based on effective rarity:

$$\text{Base}_{\text{other}} = 1.2^{(\text{EffectiveRarity} + \text{ModModifier})}$$
$$\text{PlayerBonus} = 0.01 + (0.0016 \times \text{Level}_{\text{item}})$$
$$\text{Stat}_{\text{secondary}} = \text{Base}_{\text{other}} \times \text{PlayerBonus} \times 0.10$$

Specific secondary stats incorporate a deterministic variation derived from the item modifier:

- **Haste**: $\text{Haste} = \text{Stat}_{\text{secondary}} \times \left(1 + \frac{\text{PyroRand}(\text{mod})}{32768}\right)$
- **Regeneration**: $\text{Regen} = \text{Stat}_{\text{secondary}} \times \left(1 + \frac{\text{PyroRand}(\text{mod} + 343)}{32768}\right)$
- **Critical Strike**: $\text{Crit} = \text{Stat}_{\text{secondary}} \times \left(1 + \frac{\text{PyroRand}(\text{mod} + 153)}{32768}\right)$

---

## Economy & Gold Formulas

### Shop Item Buying Price
Equipment purchase prices scale quadratically with item level:

$$\text{Price}_{\text{gear}} = (\text{BasePrice} \times \text{Level}_{\text{item}})^2$$

For category 15 items (special commodities):

$$\text{Price}_{\text{cat15}} = \text{BasePrice}^2 \times 2 \times (1 + D_{\text{region}})$$

### Gold Bag Value
Gold bags reward gold proportional to the player's distance from the home region:

$$\text{Gold}_{\text{bag}} = 100 \times (D_{\text{region}} + 1)$$

### Creature Gold Drops
The amount of gold dropped by slain enemies is directly set to the creature's level:

$$\text{Gold}_{\text{drop}} = \max(1, \text{Level}_{\text{creature}})$$

---

## Pseudo-Random Number Generator (PyroRand)

To ensure deterministic, reproducible level variations without storing additional state, a Linear Congruential Generator (LCG) is used:

$$\text{seed}_{n+1} = (\text{seed}_n \times 1103515245 + 12345) \pmod{2^{64}}$$
$$\text{PyroRand}(\text{seed}) = \left\lfloor \frac{\text{seed}_{n+1}}{65536} \right\rfloor \bmod 32768$$
